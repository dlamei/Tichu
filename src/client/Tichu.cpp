#include "Tichu.h"

#include "Renderer/renderer.h"

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
        send_message(ClientMsg(_connection_data.id, fold_req{}));
    }
    if (_game_panel_data.pressed_play) {
        _game_panel_data.pressed_play = false;
        auto &selected_cards = _game_panel_data.selected_cards;
        auto msg = play_combi_req{ CardCombination({selected_cards.begin(), selected_cards.end()}) };
        send_message(ClientMsg(_connection_data.id, msg));
    }

}

std::optional<ServerMsg> parse_message(const std::string &msg) {
    try {
        ServerMsg server_msg;
        json data = json::parse(msg);
        DEBUG("received: {}", data.dump(4));
        from_json(data, server_msg);
        return server_msg;
    } catch (const std::exception &e) {
        ERROR("failed to parse message from server:\n{}\n{}", msg, e.what());
        return {};
    }
}

void listen_to_messages(sockpp::tcp_connector &connection, MessageQueue<ServerMsg> *queue) {
    ssize_t count{};
    char msg_size_str[sizeof(int) * 2]{};

    while (true) {
        // read the length of the message
        count = connection.read_n(msg_size_str, sizeof(int) * 2);
        if (count != sizeof(int) * 2) break;

        int size;
        try {
            size = (int) std::stoul(msg_size_str, nullptr, 16); // 16 for hexadecimal
        } catch (std::exception &e) {
            // maybe delimiter so we can try to recover, but since its tcp not sure if necessary
            ERROR("while trying to parse message size from string: {}", msg_size_str);
            break;
        }

        // skip the ':' after the message size
        char c{};
        connection.read_n(&c, 1);
        ASSERT(c == ':', "invalid message format");

        std::vector<char> buffer;
        buffer.resize(size);
        connection.read_n(buffer.data(), size);
        std::string message = std::string(buffer.begin(), buffer.end());

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
        show_msg(MessageType::Warn, std::format("Failed to resolve address: {}", e.what()));
        return;
    }

    if (!_connection.connect(address)) {
        show_msg(MessageType::Warn, "Failed to connect to server:\n " + address.to_string());
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
                show_msg(MessageType::Error, std::format("unknown ServerMsgType: {} was not handled!", (int)msg.get_type()));
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

