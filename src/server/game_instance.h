/*! \class GameInstance
    \brief Tool to maintain a game session.
    

 The GameInstance class is a wrapper around the GameState of an active instance of the game.
 This class contains functions to modify the contained GameState.
*/

#ifndef TICHU_GAME_H
#define TICHU_GAME_H

#include <vector>
#include <string>
#include <mutex>

#include "../common/game_state/player/player.h"
#include "../common/game_state/game_state.h"

class GameInstance {

private:
    GameState _game_state;

    bool is_player_allowed_to_play(const Player &player);

    inline static std::mutex modification_lock;

public:
    GameInstance();

    const UUID &get_id() { return _game_state.get_id(); }

    const GameState &get_game_state();

    bool is_full();

    bool is_started();

    bool is_finished();

    /** 
     * game update functions
    */ 
    bool start_game(player_ptr player, std::string &err);

    bool try_add_player(player_ptr new_player, std::string &err);

    bool try_remove_player(player_ptr player, std::string &err);

    bool play_combi(const player_ptr& player, CardCombination &combi, std::string &err, std::optional<Card> wish = {});

    bool call_grand_tichu(const player_ptr& player, Tichu tichu, std::string &err);

    bool call_small_tichu(const player_ptr& player, Tichu tichu, std::string &err);

    bool swap_cards(const player_ptr& player, const std::vector<Card> cards, std::string &err);

    bool dragon_selection(const player_ptr& player, UUID selected_player, std::string &err);

};


#endif //TICHU_GAME_H

