#include "Tichu.h"

#include "Renderer/renderer.h"
#include "../common/listener.h"

#include <imgui.h>

void TichuGame::send_message(const ClientMsg &msg) {
    if (_connection.is_connected()) {
        json data;
        to_json(data, msg);
        auto msg_str = data.dump();
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << (int) msg_str.size();
        ss << ':' << msg_str;
        _connection.write(ss.str());
    } else {
        WARN("called send without an active connection");
    }
}


void TichuGame::on_attach() {
    sockpp::socket_initializer::initialize();
    GamePanel::load_textures();
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
    if (_connection_data.connect) {
        // reset button press
        _connection_data.connect = false;

        if (_connection_data.validate()) {
            _game_panel_data.player_id = _connection_data.id;
            connect_to_server();
        } else {
            show_msg(MessageType::Info, "invalid input");
        }
    }

    if (_game_panel_data.pressed_start_game) {
        _game_panel_data.pressed_start_game = false;
        send_message(ClientMsg(_connection_data.id, start_game_req{}));
        _game_panel_data.panel_state = GamePanel::GAME;
    }

    if (_game_panel_data.pressed_fold) {
        _game_panel_data.pressed_fold = false;
        send_message(ClientMsg(_connection_data.id, play_combi_req{}));
    }
    if (_game_panel_data.pressed_play) {
        _game_panel_data.pressed_play = false;
        auto &selected_cards = _game_panel_data.selected_cards;
        auto msg = play_combi_req{ CardCombination({selected_cards.begin(), selected_cards.end()}) };
        send_message(ClientMsg(_connection_data.id, msg));
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
        show_msg(MessageType::Warn, "Failed to connect to server:\n " + address.to_string());
        return;
    }

    _connection_data.status = "Connected to " + address.to_string();
    _listener = std::thread(tcp_listener<ServerMsg>, _connection.clone(), parse_message, &_server_msgs);

    // send join request after listener is created
    auto client_req = ClientMsg(_connection_data.id, join_game_req{.player_name = _connection_data.name});
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
    // change the state of the game panel whether the game has started or not
    if (data.state.is_started()) {
        _game_panel_data.panel_state = GamePanel::GAME;
    } else {
        _game_panel_data.panel_state = GamePanel::LOBBY;
    }

    _game_panel_data.game_state = data.state;
}

