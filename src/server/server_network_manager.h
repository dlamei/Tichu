//
// Created by Manuel on 12.02.2021.
//
// The server_network_manager handles all incoming messages and offers functionality to broadcast messages
// to all connected players of a game.

#ifndef TICHU_SERVER_NETWORK_MANAGER_H
#define TICHU_SERVER_NETWORK_MANAGER_H

#include <thread>
#include <functional>
#include <unordered_map>
#include <shared_mutex>

#include "sockpp/tcp_socket.h"
#include "sockpp/tcp_connector.h"
#include "sockpp/tcp_acceptor.h"

#include "../common/Messages.h"
#include "../common/game_state/player/Player.h"
#include "../common/game_state/GameState.h"

class server_network_manager {
private:

    inline static server_network_manager *_instance;
    inline static std::shared_mutex _rw_lock;
    inline static sockpp::tcp_acceptor _acc;

    inline static std::unordered_map<UUID, std::string> _player_id_to_address;
    inline static std::unordered_map<std::string, sockpp::tcp_socket> _address_to_socket;

    void connect(const std::string &url, const uint16_t port);

    static void listener_loop();

    static void read_message(sockpp::tcp_socket socket,
                             const std::function<void(const std::string &,
                                                      const sockpp::tcp_socket::addr_t &)> &message_handler);

    static void handle_incoming_message(const std::string &msg, const sockpp::tcp_socket::addr_t &peer_address);

    static ssize_t send_message(const std::string &msg, const std::string &address);

public:
    server_network_manager();

    ~server_network_manager();

    static void broadcast_single_message(ServerMsg &msg, std::vector<player_ptr> players, const Player &recipient);

    static void on_player_left(const UUID &player_id);
};


#endif //TICHU_SERVER_NETWORK_MANAGER_H
