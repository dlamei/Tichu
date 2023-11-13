#ifndef TICHUUI_TICHU_H
#define TICHUUI_TICHU_H

#include "GUI/Application.h"
#include <unordered_set>
#include <string>
#include <utility>

enum class TeamSelection {
    TEAM_1,
    TEAM_2,
    RANDOM,
};

// used for reading data from the connection panel
struct ConnectionPanelInput {
    std::string host = "127.0.0.1";
    uint32_t port = 50505;
    std::string name{};
    TeamSelection team = TeamSelection::RANDOM;
    bool connect = false;
};


void show_connection_panel(ConnectionPanelInput *input);

// a popup message box, e.g ERROR, WARNING, INFO, etc...
class MessageBox {
public:
    // title containing the imgui id, look at imgui docks for more info
    std::string title {};
    // the text contained in the MessageBox
    std::string message;
    // tells the owner of this object to discard it
    bool close {false};

    explicit MessageBox(std::string msg)
        : message(std::move(msg)), title("MessageBox###" + std::to_string(COUNT++)) {}

        // draws the imgui widget
    void on_imgui();

private:

    inline static size_t COUNT = 0;
};

class TichuGame : public Layer {
public:

    void on_attach() override;

    void on_update(TimeStep ts) override;
    void on_imgui() override;

private:

    void show_message_boxes();

    ConnectionPanelInput _connection_input{};
    // all message boxes currently shown. item should be removed if close field is true
    std::vector<MessageBox> _messages{};

};

#endif //TICHUUI_TICHU_H
