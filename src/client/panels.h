#ifndef TICHU_PANELS_H
#define TICHU_PANELS_H

#include "../common/Messages.h"
#include "../common/game_state/GameState.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>

#include "GamePanel.h"

namespace ImGuiUtils {
    void center_next_window_once();
    void center_next_label(const char *label, float alignment = 0.5f);
    void text_wrapped_centered(const std::string &text);
    void AlignForWidth(float width, float alignment = 0.5f);
}

namespace ConnectionPanel {

    enum class TeamSelection {
        TEAM_A,
        TEAM_B,
        RANDOM,
    };

// used for reading data from the connection panel
    struct Data {
        // used in panel
        std::string host = "127.0.0.1";
        uint32_t port = 50505;
        std::string name = "name";
        ConnectionPanel::TeamSelection team = ConnectionPanel::TeamSelection::TEAM_B;
        bool connect = false;
        std::string status = "not connected";

        // Player id
        UUID id = UUID::create();

        bool validate() const;
    };

    void show(Data *input);
}


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

#endif //TICHU_PANELS_H
