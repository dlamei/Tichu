#ifndef TICHU_PANELS_H
#define TICHU_PANELS_H

#include "../common/network/server_msg.h"
#include "../common/game_state/game_state.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

namespace ConnectionPanel {

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
        std::string name = "name";
        ConnectionPanel::TeamSelection team = ConnectionPanel::TeamSelection::RANDOM;
        bool connect = false;
        std::string status = "not connected";

        // player id
        UUID id = UUID::create();

        bool validate() const;
    };

    void show(ConnectionData *input);
}


enum class MessageType {
    ERROR,
    WARN,
    INFO,
};

// a popup message box, e.g ERROR, WARN, etc...
class Message {
public:
    MessageType type;
    // unique id for this message box
    size_t id;
    // the text contained in the Message
    std::string message;
    // tells the owner of this object that this message box should close
    bool should_close{false};

    explicit Message(MessageType typ, std::string msg)
            : message(std::move(msg)), id(COUNT++), type(typ) {}

    // draws the imgui widget
    void on_imgui();

    static void show_windows(std::vector<Message> *messages);

private:

    inline static size_t COUNT = 0;
};

namespace GamePanel {
    void init();
    void show();
    void update(const game_state &state);
}

#endif //TICHU_PANELS_H
