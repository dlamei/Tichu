
#ifndef TICHU_GAMEPANEL_H
#define TICHU_GAMEPANEL_H

#include <set>

#include "../common/Messages.h"
#include <sstream>

namespace GamePanel {

    // TODO: remove?
    //enum State {
    //    LOBBY,
    //    GAME,
    //};

    // input / output for the GamePanel
    // is also used to store e.g animation states
    struct Data {
        // read / write
        int hovered_card_index = -1;
        long begin_hover_time = -1.f;
        long spread_anim_start = -1.f;
        long begin_card_collect_anim = -1.f;
        std::set<Card> selected_cards{};

        //states of all the buttons
        bool pressed_fold {false};
        bool pressed_play {false};
        bool pressed_start_game{false};
        bool grand_tichu_pressed {false};
        bool pass_grand_tichu_pressed{false};

        // read only
        std::optional<UUID> player_id{};
        GameState game_state{};
        GameState prev_game_state{};
        bool state_updated = false;
        //std::string log_buffer{};
        std::vector<std::pair<Event, std::string>> events_log{};
        //State panel_state {State::LOBBY};
    };

    void load_textures();

    // pass in pointer if the function will modify the value, otherwise const reference
    void show(Data *data);

} // GamePanel

#endif //TICHU_GAMEPANEL_H
