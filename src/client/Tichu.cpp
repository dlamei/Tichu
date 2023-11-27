#include "Tichu.h"

#include "GUI/renderer.h"
#include "../common/network/ClientMsg.h"


void TichuGame::send_message(const ClientMsg &msg) {
    if (_connection.is_connected()) {
        auto msg_str = json_utils::to_string(*msg.to_json());
        msg_str = std::to_string(msg_str.size()) + ":" + msg_str;
        _connection.write(msg_str);
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
        _connection.shutdown();
        _listener.join();
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
            show_msg(MessageType::WARN, "invalid input");
        }
    }

    if (_game_panel_data.pressed_start_game) {
        _game_panel_data.pressed_start_game = false;
        send_message(ClientMsg(_connection_data.id, UUID(), start_game_req{}));
        _game_panel_data.state = GamePanel::GAME;
    }

    if (_game_panel_data.pressed_fold) {
        _game_panel_data.pressed_fold = false;
        send_message(ClientMsg(_connection_data.id, UUID(), fold_req{}));
    }
    if (_game_panel_data.pressed_play) {
        _game_panel_data.pressed_play = false;
        auto &selected_cards = _game_panel_data.selected_cards;
        auto msg = play_combi_req{ CardCombination({selected_cards.begin(), selected_cards.end()}) };
        send_message(ClientMsg(_connection_data.id, UUID(), msg));
    }

}

std::optional<ServerMsg> parse_message(const std::string &msg) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse(msg.c_str());
    try {
        return ServerMsg::from_json(json);
    } catch (const std::exception &e) {
        ERROR("failed to parse message from server:\n{}\n{}", msg, e.what());
        return {};
    }
}

void listen_to_messages(sockpp::tcp_connector &connection, MessageQueue<ServerMsg> *queue) {
    ssize_t count{};
    char msg_size_str[MESSAGE_SIZE_LENGTH];
    //TODO: better error handling
    while (true) {
        // read the length of the message
        count = connection.read_n(msg_size_str, MESSAGE_SIZE_LENGTH);
        if (count != MESSAGE_SIZE_LENGTH) break;

        int size = 0;
        try {
            size = std::stoi(msg_size_str);
        } catch (std::exception &e) {
            // maybe delimiter so we can try to recover, but since its tcp not sure if necessary
            ERROR("while trying to parse message size from string: {}", msg_size_str);
            break;
        }
        DEBUG("message size: {}", size);

        // skip the ':' after the message size
        char c{};
        connection.read_n(&c, 1);
        ASSERT(c == ':', "invalid message format");

        std::vector<char> buffer;
        buffer.resize(size);
        connection.read_n(buffer.data(), size);
        std::string message = std::string(buffer.begin(), buffer.end());
        DEBUG("message: {}", message);

        //TODO:
        // what do we do if parsing / reading failed?
        // does that not automatically mean that there is a bug in the encode / decoding
        // so not fixable at runtime ?
        auto msg = parse_message(message);
        if (msg) {
            queue->push(msg.value());
        }
    }

}

void TichuGame::connect_to_server() {
    sockpp::inet_address address;

    if (_connection.is_connected()) {
        WARN("connect_to_server was called while already connected!");
        _connection.shutdown();
        _listener.join();
    }

    try {
        address = sockpp::inet_address(_connection_data.host, _connection_data.port);
    } catch (const std::exception &e) {
        show_msg(MessageType::ERROR, std::format("Failed to resolve address: {}", e.what()));
        return;
    }

    if (!_connection.connect(address)) {
        show_msg(MessageType::ERROR, "Failed to connect to server " + address.to_string());
        return;
    }

    _connection_data.status = "Connected to " + address.to_string();

    try {
        _listener = std::thread(listen_to_messages, std::ref(_connection), &_server_msgs);
    } catch (std::exception &e) {
        ERROR("while creating listener thread: {}", e.what());
        return;
    }

    // send join request after listener is created
    auto client_req = ClientMsg(_connection_data.id, UUID(), join_game_req{.player_name = _connection_data.name});
    send_message(client_req);
}

void TichuGame::process_messages() {
    std::optional<ServerMsg> message{};
    while ((message = _server_msgs.try_pop())) {
        auto msg = message.value();

        switch (msg.get_type()) {
            case ServerMsgType::req_response: {
                auto data = msg.get_msg_data<request_response>();
                process(data);
                break;
            }
            case ServerMsgType::full_state_response: {
                auto data = msg.get_msg_data<full_state_response>();
                process(data);
                break;
            }
            default:
                show_msg(MessageType::ERROR, std::format("unknown ServerMsgType: {} was not handled!", (int)msg.get_type()));
                break;
        }
    }
}

void TichuGame::process(const request_response &data) {
    if (!data.success) {
        show_msg(MessageType::ERROR, data.err);
        return;
    }

    _state = PanelState::GAME;

    if (!data.state) {
        show_msg(MessageType::ERROR, "network: request_response did not contain state_json");
        return;
    }

    try {
        _state = PanelState::GAME;
        GamePanel::update(data.state.value());
    } catch (std::exception &e) {
        show_msg(MessageType::ERROR, "network: could not parse GameState from req_response");
    }

}

void TichuGame::process(const full_state_response &data) {

    try {
        _state = PanelState::GAME;
        if (data.state.is_started()) {
            _game_panel_data.state = GamePanel::GAME;
        } else {
            _game_panel_data.state = GamePanel::LOBBY;
        }

        GamePanel::update(data.state);
    } catch (std::exception &e) {
        show_msg(MessageType::ERROR, "network: could not parse GameState from req_response");
    }
}

