//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_GAME_H
#define LAMA_GAME_H

#include <vector>
#include <string>
#include <mutex>

#include "game_state/player/player.h"
#include "game_state/game_state.h"

class game {

private:
    game_state* _game_state;
    bool is_player_allowed_to_play(player* player);
    inline static std::mutex modification_lock;

public:
    game();
    ~game() {
        if (_game_state != nullptr) {
            delete _game_state;
        }
        _game_state = nullptr;
    }
    std::string get_id();

    game_state* get_game_state();

    bool is_full();
    bool is_started();
    bool is_finished();

#ifdef LAMA_SERVER
    // server only functions

#ifndef USE_DIFFS
    // full state version
    bool start_game(player* player, std::string& err);
    bool try_add_player(player* new_player, std::string& err);
    bool try_remove_player(player* player, std::string& err);
    bool play_card(player* player, const std::string& card_id, std::string& err);
    bool draw_card(player* player, card*& drawn_card, std::string& err);
    bool fold(player* player, std::string& err);
#else
    // diff versions
    bool start_game(player* player, object_diff& game_state_diff, std::string& err);
    bool try_add_player(player* new_player, object_diff& game_state_diff, std::string& err);
    bool try_remove_player(player* player, object_diff& game_state_diff, std::string& err);
    bool play_card(player* player, const std::string& card_id, object_diff& game_state_diff, std::string& err);
    bool draw_card(player* player, card*& drawn_card, object_diff& game_state_diff, std::string& err);
    bool fold(player* player, object_diff& game_state_diff, std::string& err);
#endif

#endif
};


#endif //LAMA_GAME_H

