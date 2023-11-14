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


void show_connection_panel(ConnectionPanelInput &input);

enum class MessageType {
    ERROR,
    WARN,
    INFO,
};

// a popup message box, e.g ERROR, WARNING, INFO, etc...
class MessageBox {
public:
    MessageType type;
    // unique id for this message box
    size_t id;
    // the text contained in the MessageBox
    std::string message;
    // tells the owner of this object that this message box should close
    bool should_close { false };

    explicit MessageBox(MessageType typ, std::string msg)
        : message(std::move(msg)), id(COUNT++), type(typ) {}

        // draws the imgui widget
    void on_imgui();

private:

    inline static size_t COUNT = 0;
};

enum Panel {
    CONNECTION_PANEL = 1 << 0,
    GAME_PANEL = 1 << 1,
};

class TichuGame : public Layer {
public:

    void on_attach() override;

    void on_update(TimeStep ts) override;
    void on_imgui() override;

private:

    void show_message_boxes();

    Panel _state {CONNECTION_PANEL};

    Texture texture;

    ConnectionPanelInput _connection_input{};
    // all message boxes currently shown. item should be removed if close field is true
    std::vector<MessageBox> _messages{};
};

#endif //TICHUUI_TICHU_H
