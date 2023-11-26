#include "Tichu.h"

#include "GUI/renderer.h"
#include "../../src/common/network/client_msg.h"
#include "../common/network/server_msg.h"


void TichuGame::on_attach() {
    sockpp::socket_initializer::initialize();
    GamePanel::init();
}

void TichuGame::on_detach() {
    if (_connection.is_connected()) {
        _connection.shutdown();
        _listener.join();
    }
}

void TichuGame::on_update(TimeStep ts) {
    process_messages();
}

void TichuGame::on_imgui() {
    // always show available messages
    Message::show_windows(&_messages);

    // connection
    if (_state == PanelState::CONNECTION) {
        ConnectionPanel::show(&_connection_data);
    }

    if (_connection_data.connect) {
        // reset button press
        _connection_data.connect = false;

        if (_connection_data.validate()) {
            connect_to_server();
        } else {
            show_msg(MessageType::WARN, "invalid input");
        }
    }


    // game
    if (_state == PanelState::GAME) {
        GamePanel::show();
    }


}

void send_message(sockpp::connector &connection, const client_msg &msg) {
    if (connection.is_connected()) {
        auto msg_str = json_utils::to_string(*msg.to_json());
        msg_str = std::to_string(msg_str.size()) + ":" + msg_str;
        connection.write(msg_str);
    } else {
        WARN("called send_message without an active connection");
    }
}

std::optional<server_msg> parse_message(const std::string &msg) {
    rapidjson::Document json = rapidjson::Document(rapidjson::kObjectType);
    json.Parse(msg.c_str());
    try {
        return server_msg::from_json(json);
    } catch (const std::exception &e) {
        ERROR("failed to parse message from server:\n{}\n{}", msg, e.what());
        return {};
    }
}

void listen_to_messages(sockpp::tcp_connector &connection, MessageQueue<server_msg> *queue) {
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
    auto client_req = client_msg(_connection_data.id, UUID(), join_game_req{.player_name = _connection_data.name});
    send_message(_connection, client_req);
}

void TichuGame::process_messages() {
    std::optional<server_msg> message{};
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
                _state = PanelState::GAME;
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
    if (!data.state_json) {
        show_msg(MessageType::ERROR, "network: request_response did not contain state_json");
        return;
    }

    try {
        game_state state = game_state::from_json(*data.state_json.value());
        GamePanel::update(state);
    } catch (std::exception &e) {
        show_msg(MessageType::ERROR, "network: could not parse game_state from req_response");
    }

}

void TichuGame::process(const full_state_response &data) {
    try {
        game_state state = game_state::from_json(*data.state_json);
        GamePanel::update(state);
    } catch (std::exception &e) {
        show_msg(MessageType::ERROR, "network: could not parse game_state from req_response");
    }
}

