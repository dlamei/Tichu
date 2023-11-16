// The game_instance class is a wrapper around the game_state of an active instance of the game.
// This class contains functions to modify the contained game_state.

#ifndef TICHU_GAME_H
#define TICHU_GAME_H

#include <vector>
#include <string>
#include <mutex>

#include "../common/game_state/player/player.h"
#include "../common/game_state/game_state.h"

class game_instance {

private:
    game_state _game_state;
    bool is_player_allowed_to_play(const player &player);
    inline static std::mutex modification_lock;

public:
    game_instance();
    const UUID &get_id() { return _game_state.get_id(); }

    const game_state &get_game_state();

    bool is_full();
    bool is_started();
    bool is_finished();

    // game update functions
    bool start_game(player_ptr player, std::string& err);
    bool try_add_player(player_ptr new_player, std::string& err);
    bool try_remove_player(player_ptr player, std::string& err);
    bool play_combi(player_ptr player, card_combination& combi, std::string& err);

};


#endif //TICHU_GAME_H

