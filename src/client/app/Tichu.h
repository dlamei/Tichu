#ifndef TICHUUI_TICHU_H
#define TICHUUI_TICHU_H

#include <string>

enum class TeamSelection {
    TEAM_1,
    TEAM_2,
    RANDOM,
};

struct ConnectionPanelInput {
    std::string host = "127.0.0.1";
    uint32_t port = 50505;
    std::string name{};
    TeamSelection team = TeamSelection::RANDOM;
    bool connect = false;
};

void show_connection_panel(ConnectionPanelInput *input);

#endif //TICHUUI_TICHU_H
