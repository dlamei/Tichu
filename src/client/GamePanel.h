
#ifndef TICHU_GAMEPANEL_H
#define TICHU_GAMEPANEL_H

#include <set>

#include "../common/Messages.h"

namespace GamePanel {

    enum State {
        LOBBY,
        GAME,
    };

    // input / output for the GamePanel
    struct Data {
        // read / write
        int hovered_card_index = -1;
        long begin_hover_time = -1.f;
        std::set<Card> selected_cards{};
        bool pressed_fold {false};
        bool pressed_play {false};
        bool pressed_start_game{false};

        // read only
        std::optional<UUID> player_id{};
        GameState game_state{};
        State panel_state {State::LOBBY};
    };

    void load_textures();

    // pass in pointer if the function will modify the value, otherwise const reference
    void show(Data *data);

} // GamePanel

#endif //TICHU_GAMEPANEL_H
