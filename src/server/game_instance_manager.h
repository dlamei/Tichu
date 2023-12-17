/*! \class game_instance_manager
    \brief Stores all active games and offers game instance functionalities.

 The game_instance_manager only exists on the server side. It stores all currently active games and offers
 functionality to retrieve game instances by id and adding players to games.
 If a new Player requests to join a game but no valid GameInstance is available, then this class
 will generate a new GameInstance and add it to the unordered_map of (active) game instances.
*/

#ifndef TICHU_GAME_INSTANCE_MANAGER_H
#define TICHU_GAME_INSTANCE_MANAGER_H

#include <string>
#include <shared_mutex>
#include <unordered_map>
#include <memory>

#include "GameInstance.h"

using game_instance_ptr = std::shared_ptr<GameInstance>;

class game_instance_manager {

private:

    inline static std::shared_mutex games_lut_lock;
    static std::unordered_map<UUID, game_instance_ptr> games_lut;

    static std::shared_ptr<GameInstance> create_new_game();

    static std::shared_ptr<GameInstance> find_joinable_game_instance();

public:

    /** returns true if the desired GameInstance '_game_id' was found or false otherwise.
    * The found game instance is written into game_instance_ptr.
    */
    static std::optional<game_instance_ptr> try_get_game_instance(const UUID &game_id);

    /** returns true if the desired Player '_player_id' was found and is connected to a GameInstance.
    * The found Player and GameInstance will be written into 'Player' and 'game_instance_ptr'
    */
    static std::optional<std::tuple<std::shared_ptr<Player>, game_instance_ptr >>
    try_get_player_and_game_instance(const UUID &player_id, std::string &err);

    /** Try to add 'Player' to any game. Returns true if 'Player' is successfully added to a GameInstance.
     * The joined GameInstance will be written into 'game_instance_ptr'.
    */
    static std::optional<game_instance_ptr> try_add_player_to_any_game(player_ptr player, std::string &err);

    /** 
     * Try to add 'Player' to the provided 'game_instance_ptr'. Returns true if success and false otherwise.
    */
    static bool try_add_player(player_ptr player, GameInstance &game_instance_ptr, std::string &err);


    static bool try_remove_player(player_ptr player, const UUID &game_id, std::string &err);

    static bool try_remove_player(player_ptr player, GameInstance &game_instance_ptr, std::string &err);

};


#endif //TICHU_GAME_INSTANCE_MANAGER_H
