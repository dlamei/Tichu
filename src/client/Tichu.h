#ifndef TICHUUI_TICHU_H
#define TICHUUI_TICHU_H

#include "sockpp/tcp_connector.h"
#undef ERROR
#include "GUI/Application.h"
#include "../../src/common/serialization/serializable.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

template<typename T>
class MessageQueue {
public:

    void push(T item) {
        std::unique_lock<std::mutex> lock(_mutex);
        _queue.push(item);
    }

    std::optional<T> poll_message() {
        std::unique_lock<std::mutex> lock(_mutex);

        if (_queue.empty()) {
            return {};
        } else {
            return _queue.pop();
        }
    }

private:
    std::queue<T> _queue;
    std::mutex _mutex;
};

enum class TeamSelection {
    TEAM_1,
    TEAM_2,
    RANDOM,
};

// used for reading data from the connection panel
struct ConnectionData {
    // used in panel
    std::string host = "127.0.0.1";
    uint32_t port = 50505;
    std::string name{};
    TeamSelection team = TeamSelection::RANDOM;
    bool connect = false;
    std::string status = "not connected";

    // actual connection
    sockpp::tcp_connector connection{};
    // player id
    UUID id = UUID::create();

    bool validate();
};



void show_connection_panel(ConnectionData &input);

enum class MessageType {
    ERROR,
    WARN,
    INFO,
};

// a popup message box, e.g ERROR, WARN_LOG, etc...
class MessageWindow {
public:
    MessageType type;
    // unique id for this message box
    size_t id;
    // the text contained in the MessageWindow
    std::string message;
    // tells the owner of this object that this message box should close
    bool should_close { false };

    explicit MessageWindow(MessageType typ, std::string msg)
        : message(std::move(msg)), id(COUNT++), type(typ) {}

        // draws the imgui widget
    void on_imgui();

private:

    inline static size_t COUNT = 0;
};

enum Panel {
    CONNECTION_PANEL = 1 << 0,
    GAME_PANEL = 1 << 2,
};

class TichuGame : public Layer {
public:

    void on_attach() override;
    void on_detach() override;

    void on_update(TimeStep ts) override;
    void on_imgui() override;

    void show_msg(MessageType typ, const std::string &msg) {
        _messages.emplace_back(typ, msg);
    }

    TichuGame() = default;
    TichuGame(const TichuGame&) = delete;

private:

    void show_message_boxes();

    void connect_to_server();

    Panel _state {CONNECTION_PANEL};

    // temp
    Texture texture;

    // connection data
    ConnectionData _connection_data{};

    MessageQueue<std::string> _server_msgs{};
    std::thread _listener {};

    // all message boxes currently shown. item should be removed if close field is true
    std::vector<MessageWindow> _messages{};
};

#endif //TICHUUI_TICHU_H
