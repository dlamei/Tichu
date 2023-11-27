
#ifndef TICHU_GAMEPANEL_H
#define TICHU_GAMEPANEL_H

#include <set>

#include "../common/network/ServerMsg.h"
#include "../common/game_state/GameState.h"

namespace GamePanel {

    enum State {
        LOBBY,
        GAME,
    };

    // modifiable variables for the GamePanel
    struct Data {
        State state {State::LOBBY};
        int hovered_card_index = -1;
        std::set<Card> selected_cards{};
        bool pressed_fold {false};
        bool pressed_play {false};
        bool pressed_start_game{false};

        std::optional<UUID> player_id{};
    };

    void load_textures();

    void show(Data *data);
    void update(const GameState &state);

} // GamePanel

#endif //TICHU_GAMEPANEL_H
