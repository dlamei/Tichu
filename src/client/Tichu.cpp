#include "Tichu.h"

#include "Renderer/renderer.h"
#include "../common/listener.h"
#include <sstream>

#include <imgui.h>

void TichuGame::send_message(const ClientMsg &msg) {
    if (_connection.is_connected()) {
        json data;
        to_json(data, msg);
        auto msg_str = data.dump();
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << (int) msg_str.size();
        ss << ':' << msg_str;
        DEBUG("sending: {}", ss.str());
        _connection.write(ss.str());
    } else {
        WARN("called send without an active connection");
    }
}


void TichuGame::on_attach() {
    sockpp::socket_initializer::initialize();
    GamePanel::load_textures();
    _connection_data.name = Application::get_info().player_name;
    _auto_connect = Application::get_info().auto_connect;
}

void TichuGame::on_detach() {
    if (_connection.is_connected()) {
        try {
            _connection.shutdown();
            _listener.join();
        } catch (std::exception &e) {
            WARN("while trying to close game: {}", e.what());
        }
    }
}

void TichuGame::on_update(TimeStep ts) {
    process_messages();
    show();
    handle_gui_output();
}

void TichuGame::show() {
    // always show available messages
    Message::show_windows(&_messages);

    switch (_state) {
        case PanelState::CONNECTION:
            ConnectionPanel::show(&_connection_data);
            break;
        case PanelState::GAME:
            GamePanel::show(&_game_panel_data);
            break;
    }
}

void TichuGame::handle_gui_output() {
    if (_auto_connect) _connection_data.connect = true;

    if (_state == PanelState::CONNECTION && _connection_data.connect) {
        // reset button press
        _connection_data.connect = false;

        if (_connection_data.validate()) {
            _game_panel_data.player_id = _connection_data.id;
            connect_to_server();
        } else {
            show_msg(MessageType::Info, "invalid input");
        }
    }

    // press start game in lobby
    if (_game_panel_data.pressed_start_game) {
        _game_panel_data.pressed_start_game = false;
        send_message(ClientMsg(_connection_data.id, start_game_req{}));
    }

    // announce grand tichu
    if (_game_panel_data.pressed_grand_tichu) {
        _game_panel_data.pressed_grand_tichu = false;
        _game_panel_data.wait_for_others_grand_tichu = true;
        send_message(ClientMsg(_connection_data.id, grand_tichu_req{ Tichu::GRAND_TICHU }));
    }

    // dont announce grand tichu
    if (_game_panel_data.pressed_pass_grand_tichu) {
        _game_panel_data.pressed_pass_grand_tichu = false;
        _game_panel_data.wait_for_others_grand_tichu = true;
        send_message(ClientMsg(_connection_data.id, grand_tichu_req{ Tichu::NONE }));
    }

    // skip round
    if (_game_panel_data.pressed_fold) {
        _game_panel_data.pressed_fold = false;
        send_message(ClientMsg(_connection_data.id, play_combi_req{}));
    }

    // swap player cards
    if (_game_panel_data.pressed_swap) {
        _game_panel_data.pressed_swap = false;
        _game_panel_data.wait_for_others_swap = true;
        auto cards = _game_panel_data.cards_for_swapping;
        send_message(ClientMsg(_connection_data.id, swap_req{ { cards.begin(), cards.end() } }));
    }

    if (_game_panel_data.pressed_small_tichu) {
        _game_panel_data.pressed_small_tichu = false;
        send_message(ClientMsg(_connection_data.id, small_tichu_req{}));
    }

    if (_game_panel_data.pressed_select) {
        _game_panel_data.pressed_select = false;
        send_message(ClientMsg(_connection_data.id, dragon_req{_game_panel_data.selected_player->get_id()}));
    }

    if (_game_panel_data.pressed_start_again) {
        _game_panel_data.pressed_start_again = false;
        send_message(ClientMsg(_connection_data.id, start_game_req{}));
    }

    if (_game_panel_data.pressed_close) {
        exit(0);
    }

    // play selected cards
    if (_game_panel_data.pressed_play || _game_panel_data.pressed_play_bomb) {
        _game_panel_data.pressed_play = false;
        _game_panel_data.pressed_play_bomb = false;
        _game_panel_data.can_play_bomb = false;
        auto &selected_cards = _game_panel_data.selected_cards;
        auto msg = play_combi_req{ CardCombination({selected_cards.begin(), selected_cards.end()}) };
        send_message(ClientMsg(_connection_data.id, msg));
        selected_cards.clear();
    }

}

ServerMsg parse_message(const std::string &msg) {
    ServerMsg server_msg;
    json data = json::parse(msg);
    DEBUG("received: {}", data.dump(4));
    from_json(data, server_msg);
    return server_msg;
}

void TichuGame::connect_to_server() {
    sockpp::inet_address address;

    if (_connection.is_connected()) {
        WARN("connect_to_server was called while already connected!");
        _connection.shutdown();
        _listener.join();
    }

    address = sockpp::inet_address(_connection_data.host, _connection_data.port);

    if (!_connection.connect(address)) {
        if (!_auto_connect) {
            show_msg(MessageType::Warn, "Failed to connect to server:\n " + address.to_string());
        }
        return;
    }

    _auto_connect = false;

    _connection_data.status = "Connected to " + address.to_string();
    _listener = std::thread(tcp_listener<ServerMsg>, _connection.clone(), parse_message, &_server_msgs);

    // team stuff
    int team = 0;
    if(_connection_data.team == ConnectionPanel::TeamSelection::TEAM_A) {
        team = 1;
    } else if(_connection_data.team == ConnectionPanel::TeamSelection::TEAM_B) {
        team = 2;
    } 


    // send join request after listener is created
    auto client_req = ClientMsg(_connection_data.id, join_game_req{_connection_data.name, team});
    send_message(client_req);
}

void TichuGame::process_messages() {
    std::optional<ServerMsg> message{};
    while ((message = _server_msgs.try_pop())) {
        auto msg = message.value();

        switch (msg.get_type()) {
            case ServerMsgType::req_response: {
                auto data = msg.get_msg_data<server_message>();
                process(data);
                break;
            }
            case ServerMsgType::full_state: {
                auto data = msg.get_msg_data<full_state_response>();
                process(data);
                break;
            }
            default:
                std::string msg_str = "unknown ServerMsgType: " + std::to_string((int) msg.get_type());
                show_msg(MessageType::Error, msg_str);
                break;
        }
    }
}

void TichuGame::process(const server_message &data) {
    show_msg(data.type, data.msg);
}

void TichuGame::process(const full_state_response &data) {
    _state = PanelState::GAME;

    auto &events = data.events;
    for (auto &e : events) {
        _game_panel_data.events_log.emplace_back(e, e.to_string(_game_panel_data.game_state.get_players(), _game_panel_data.player_id.value()));
    }

    _game_panel_data.prev_game_state = _game_panel_data.game_state;
    _game_panel_data.game_state = data.state;
    _game_panel_data.state_updated = true;
}

