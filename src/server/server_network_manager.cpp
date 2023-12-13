// The server_network_manager handles all incoming messages and offers functionality to broadcast messages
// to all connected players of a game.

#include <sstream>
#include "server_network_manager.h"
#include "request_handler.h"

const std::string default_server_host = "127.0.0.1";
const unsigned int default_port = 50505;


server_network_manager::server_network_manager() {
    if (_instance == nullptr) {
        _instance = this;
    }
    init_logger();
    sockpp::socket_initializer::initialize(); // Required to initialise sockpp
    this->connect(default_server_host, default_port);   // variables from "default.conf"
}

server_network_manager::~server_network_manager() = default;

void server_network_manager::connect(const std::string &url, const uint16_t port) {
    this->_acc = sockpp::tcp_acceptor(port);

    if (!_acc) {
        std::cerr << "Error creating the acceptor: " << _acc.last_error_str() << std::endl;
        return;
    }

    std::cout << "Awaiting connections on port " << port << "..." << std::endl;
    listener_loop();    // start endless loop
}

void server_network_manager::listener_loop() {
    // intentional endless loop
    while (true) {
        sockpp::inet_address peer;

        // Accept a new client connection
        sockpp::tcp_socket sock = _acc.accept(&peer);
        std::cout << "Received a connection request from " << peer << std::endl;

        if (!sock) {
            std::cerr << "Error accepting incoming connection: "
                      << _acc.last_error_str() << std::endl;
        } else {
            _rw_lock.lock();
            _address_to_socket.emplace(sock.peer_address().to_string(), std::move(sock.clone()));
            _rw_lock.unlock();
            // Create a listener thread and transfer the new stream to it.
            // Incoming messages will be passed to handle_incoming_message().
            std::thread listener(
                    read_message,
                    std::move(sock),
                    handle_incoming_message);

            listener.detach();
        }
    }
}

// TODO: write listener thread once for server and client in common
// Runs in a thread and reads anything coming in on the 'socket'.
// Once a message is fully received, the string is passed on to the 'handle_incoming_message()' function
void server_network_manager::read_message(sockpp::tcp_socket socket, const std::function<void(const std::string &,
                                                                                              const sockpp::tcp_socket::addr_t &)> &message_handler) {
    sockpp::socket_initializer::initialize(); // Required to initialise sockpp

    //char buffer[512]; // 512 bytes
    char msg_size_str[sizeof(int) * 2];

    int count;
    //while ((count = socket.read(buffer, sizeof(buffer))) > 0) {
    while (true) {
        try {
            count = socket.read_n(msg_size_str, sizeof(int) * 2);
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
            socket.read_n(&c, 1);
            ASSERT(c == ':', "invalid message format");

            std::vector<char> buffer;
            buffer.resize(size);
            socket.read_n(buffer.data(), size);
            std::string message = std::string(buffer.begin(), buffer.end());

            // handle incoming message
            message_handler(message, socket.peer_address());

        } catch (std::exception &e) {
            ERROR("while reading message from {}: {}", socket.peer_address().to_string(), e.what());
        }
    }
    if (count < 0) {
        std::cout << "Read error [" << socket.last_error() << "]: "
                  << socket.last_error_str() << std::endl;
    }

    std::cout << "Closing connection to " << socket.peer_address() << std::endl;
    socket.shutdown();
}


void server_network_manager::handle_incoming_message(const std::string &msg,
                                                     const sockpp::tcp_socket::addr_t &peer_address) {
    try {
        // try to parse a json from the 'msg'
        // try to parse a ClientMsg from the json
        ClientMsg client_msg;
        from_json(json::parse(msg), client_msg);

        // check if this is a connection to a new Player
        auto player_id = client_msg.get_player_id();
        _rw_lock.lock_shared();
        if (_player_id_to_address.find(player_id) == _player_id_to_address.end()) {
            // save connection to this client
            _rw_lock.unlock_shared();
            std::cout << "New client with id " << player_id.string() << std::endl;
            _rw_lock.lock();
            _player_id_to_address.emplace(player_id, peer_address.to_string());
            _rw_lock.unlock();
        } else {
            _rw_lock.unlock_shared();
        }

        DEBUG("Received valid request : {}", msg);

        // execute client request
        std::optional<ServerMsg> res = request_handler::handle_request(client_msg);

        if( res ) {
        // transform response into a json
        json response;
        to_json(response, res.value());

        DEBUG("sending response: {}", response.dump(4));

        // send response back to client
        send_message(response.dump(), peer_address.to_string());
        } 

    } catch (const std::exception &e) {
        std::cerr << "Failed to execute client request. Content was :\n"
                  << msg << std::endl
                  << "Error was " << e.what() << std::endl;
    }
}


void server_network_manager::on_player_left(const UUID &player_id) {
    _rw_lock.lock();
    std::string address = _player_id_to_address[player_id];
    _player_id_to_address.erase(player_id);
    _address_to_socket.erase(address);
    _rw_lock.unlock();
}

ssize_t server_network_manager::send_message(const std::string &msg, const std::string &address) {

    std::stringstream ss_msg;
    ss_msg << std::setfill('0') << std::setw(sizeof(int) * 2) << std::hex << (int) msg.size();
    ss_msg << ':' << msg; // prepend message length
    return _address_to_socket.at(address).write(ss_msg.str());
}


void server_network_manager::broadcast_single_message(ServerMsg &msg, std::vector<player_ptr> players, const Player &recipient) {
    json data;
    to_json(data, msg);
    DEBUG("broadcast_single_message: {}", data.dump(1));

    _rw_lock.lock_shared();
    // send object_diff to all requested players
    try {
        send_message(data.dump(), _player_id_to_address.at(recipient.get_id()));
    } catch (std::exception &e) {
        std::cerr << "Encountered error when sending state update: " << e.what() << std::endl;
    }
    _rw_lock.unlock_shared();

}




