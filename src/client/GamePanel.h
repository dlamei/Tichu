
#ifndef TICHU_GAMEPANEL_H
#define TICHU_GAMEPANEL_H

#include <set>

#include "../common/Messages.h"
#include <sstream>

namespace GamePanel {

    /**
     * \brief output for panels that allow for swapping cards
    */ 
    struct SelectionData {
        /** max number of selections */
        int n_selections{0};
        /** indices of selected cards */
        std::deque<int> selected{};
    };

    /**
     * \brief input / output for the GamePanel 
     * 
     * It is also used to store e.g animation states.
    */ 
    struct Data {
        // read / write
        int hovered_card_index = -1;
        long long begin_hover_time = -1;
        long long spread_anim_start = -1;
        long long begin_card_collect_anim = -1;
        std::set<Card> selected_cards{};
        std::optional<Card> wish{};
        bool can_play_bomb{false};

        bool selected_majong{true};
        bool show_majong_selection{false};
        SelectionData mahjong_wish{};

        int my_index = -1;

        /**
         * SelectionData for each player (minus local player)
         * used to store data when displaying the swap window
         * uses relative indexing
        */  
        std::array<SelectionData, 3> swap_window_data{};

        /** store cards to be swapped at the beginning*/
        std::array<Card, 3> cards_for_swapping{};

        std::optional<Player> selected_player{};

        //states of all the buttons
        bool pressed_fold {false};
        bool pressed_play {false};
        bool pressed_start_game{false};
        bool pressed_grand_tichu {false};
        bool pressed_small_tichu {false};
        bool pressed_pass_grand_tichu{false};
        bool pressed_swap{false};
        bool pressed_select{false};
        bool pressed_start_again{false};
        bool pressed_close{false};
        bool pressed_play_bomb{false};

        bool wait_for_others_grand_tichu = false;
        bool wait_for_others_swap = false;

        // read only
        std::optional<UUID> player_id{};
        GameState game_state{};
        GameState prev_game_state{};
        bool state_updated = false;
        std::deque<std::pair<Event, std::string>> events_log{};
    };

    void load_textures();

    /** pass in pointer if the function will modify the value, otherwise const reference */
    void show(Data *data);

} // GamePanel

#endif //TICHU_GAMEPANEL_H
