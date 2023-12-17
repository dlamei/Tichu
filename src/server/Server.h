#ifndef TICHU_SERVER_H
#define TICHU_SERVER_H

/*! \class Server
    \brief Represents the Tichu game server.

    The Server class manages the Tichu game server, handling new connections, processing messages,
    and managing game instances. It provides functionality to send and broadcast messages to connected clients.
*/

#include <sockpp/tcp_socket.h>
#include <sockpp/tcp_acceptor.h>
#include <sockpp/tcp_connector.h>
#include <vector>

#include "../common/utils.h"
#include "../common/Messages.h"
#include "GameInstance.h"

/*! \struct ConnectionData
    \brief Represents data associated with a client connection.

    This structure holds information about a connected client, including the associated player, address, socket, and a listener thread.

    \var ConnectionData::player
        \brief An optional field representing the associated player.
    \var ConnectionData::address
        \brief A string representing the address of the client.
    \var ConnectionData::socket
        \brief A shared pointer to a TCP socket used for communication with the client.
    \var ConnectionData::_listener
        \brief A shared pointer to the listener thread associated with the client connection.
*/
struct ConnectionData {
    std::optional<Player> player;
    std::string address{};
    std::shared_ptr<sockpp::tcp_socket> socket{};
    std::shared_ptr<std::thread> _listener{};
};

/*! \struct GameData
    \brief Represents data associated with a game instance.

    This structure holds information about a game instance, including the associated game object and a unique identifier.

    \var GameData::game
        \brief An instance of the GameInstance class representing the game.
    \var GameData::id
        \brief A UUID representing the unique identifier of the game.
*/
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
