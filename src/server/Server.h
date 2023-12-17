#ifndef TICHU_SERVER_H
#define TICHU_SERVER_H

/*! \class Server
    \brief ##########TODO

 ##########################TODO
*/

#include <sockpp/tcp_socket.h>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_connector.h>
#include <vector>

#include "../common/utils.h"
#include "../common/Messages.h"
#include "GameInstance.h"

struct ConnectionData {
    std::optional<Player> player;
    std::string address{};
    std::shared_ptr<sockpp::tcp_socket> socket{};
    std::shared_ptr<std::thread> _listener{};
};

struct GameData {
    GameInstance game;
    UUID id;
};

class Server {
public:
    explicit Server(uint16_t port);
    void run();
private:

    void handle_new_connections();
    void send_message(const ServerMsg &msg, const ConnectionData &client);
    void broadcast_message(const ServerMsg &msg);

    void process_messages();

    // list of connected clients
    std::vector<ConnectionData> _clients;
    std::vector<GameData> _game_instances;

    // waits for incoming connections
    std::thread _connection_listener;
    // queue of clients trying to connect
    MessageQueue<ConnectionData> _connection_req;
    MessageQueue<ClientMsg> _client_msgs;
};


#endif //TICHU_SERVER_H
