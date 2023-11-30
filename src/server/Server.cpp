#include "Server.h"
#include "../common/listener.h"
#include "request_handler.h"

void await_connections(const uint16_t port, MessageQueue<ConnectionData> *queue) {
    auto _acceptor = sockpp::tcp_acceptor(port);

    if (!_acceptor) {
        ERROR("while creating the acceptor: {}", _acceptor.last_error_str());
    }
    INFO("listening for connections on port: {}", port);

    while (true) {
        // wait for new client
        sockpp::inet_address peer;
        auto sock = std::make_shared<sockpp::tcp_socket>(_acceptor.accept(&peer));
        DEBUG("connection request from: {}", peer.to_string());

        if (!sock) {
            ERROR("could not accept connection: {}", _acceptor.last_error_str());
            continue;
        }

        auto connection = ConnectionData {.address = sock->peer_address().to_string(), .socket = sock };
        queue->push(connection);
    }
}

ClientMsg parse_message(const std::string &msg) {
    ClientMsg client_msg;
    json data = json::parse(msg);
    DEBUG("received: {}", data.dump(4));
    from_json(data, client_msg);
    return client_msg;
}

Server::Server(const uint16_t port) {
    init_logger();
    sockpp::socket_initializer::initialize();
    _connection_listener = std::thread(await_connections, port, &_connection_req);
}

void Server::run() {
    while (true) {
        handle_new_connections();
        process_messages();
    }
}

void Server::handle_new_connections() {
    std::optional<ConnectionData> connection{};
    // create a listener thread for every client trying to connect and store the connection info in _clients
    while ((connection = _connection_req.try_pop())) {
        connection->_listener = std::make_shared<std::thread>(tcp_listener<ClientMsg>, connection->socket->clone(), parse_message, &_client_msgs);
        _clients.push_back(connection.value());
    }
}

void Server::process_messages() {
    std::optional<ClientMsg> message{};
    while ((message = _client_msgs.try_pop())) {
        auto msg = message.value();
        const auto &player_id = msg.get_player_id();

        switch (msg.get_type()) {
            case ClientMsgType::join_game: {
                std::string player_name = msg.get_msg_data<join_game_req>().player_name;
                break;
            }
            case ClientMsgType::start_game: {
                break;
            }
            case ClientMsgType::play_combi: {
                break;
            }
            case ClientMsgType::fold: {
                break;
            }
            case ClientMsgType::ping: {
                break;
            }
        }
    }
}

void Server::send_message(const ServerMsg &msg, const ConnectionData &client) {
    json msg_json;
    to_json(msg_json, msg);
    auto msg_str = msg_json.dump();
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << (int)msg_str.size();
    ss << ':' << msg_str;

    try {
        client.socket->write(ss.str());
    } catch (std::exception &e) {
        ERROR("while sending message to: {}\nmessage: {}", client.address, ss.str());
    }
}

void Server::broadcast_message(const ServerMsg &msg) {
    json data;
    to_json(data, msg);
    DEBUG("broadcasting: {}", data.dump(4));

    for (const auto &c : _clients) {
        send_message(msg, c);
    }
}

