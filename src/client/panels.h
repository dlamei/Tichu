#ifndef TICHU_PANELS_H
#define TICHU_PANELS_H


/*! \class Message
    \brief Represents a popup message box.

 The Message class is responsible for representing a popup message box
 in the Tichu game UI. It includes information such as the message type, unique ID, 
 message content, and a flag to indicate whether the message box should close.

 The class also provides functionality to draw the ImGui widget and manage 
 the display of multiple message boxes.
*/

#include "../common/Messages.h"
#include "../common/game_state/GameState.h"
#include <unordered_set>
#include <string>
#include <utility>
#include <queue>
#include <chrono>
#include <functional>

#include "GamePanel.h"

namespace ImGuiUtils {
    void center_next_in_window(int cond);
    void center_next_in_viewport(int cond);
    void center_next_label(const char *label, float alignment = 0.5f);
    void text_wrapped_centered(const std::string &text);
    void item_grid(const char *label, int n_items, float item_width, const std::function<void (int)>& draw_item, int flags = 0);
}

namespace ConnectionPanel {

    enum class TeamSelection {
        TEAM_A,
        TEAM_B,
        RANDOM,
    };

/**
 * \brief used for reading data from the connection panel
*/ 
    struct Data {
        // used in panel
        std::string host = "127.0.0.1";
        uint32_t port = 50505;
        std::string name = "name";
        ConnectionPanel::TeamSelection team = ConnectionPanel::TeamSelection::TEAM_B;
        bool connect = false;
        std::string status = "not connected";

        /**
         * Player id
        */ 
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
