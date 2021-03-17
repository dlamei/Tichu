//
// Created by Manuel on 29.01.2021.
//
// The game_instance_manager only exists on the server side. It stores all currently active games and offers
// functionality to query game instances and adding players to games.
//

#ifndef LAMA_GAME_INSTANCE_MANAGER_H
#define LAMA_GAME_INSTANCE_MANAGER_H

#include <string>
#include <shared_mutex>
#include <unordered_map>

#include "../game.h"

class game_instance_manager {

private:

    inline static std::shared_mutex games_lut_lock;
    static std::unordered_map<std::string, game*> games_lut;

    static game* create_new_game();
    static game* find_joinable_game_instance();

public:

    // returns true if the desired game instance 'game_id' was found or false otherwise.
    // The found game instance is written into game_instance_ptr.
    // if the passed 'game_id' is an empty string, any non-full, non-started game_instance will be written into game_instance_ptr
    static bool try_get_game_instance(const std::string& game_id, game*& game_instance_ptr);
    static bool try_get_player_and_game_instance(const std::string& player_id, player*& player, game*& game_instance, std::string& err);

#ifndef USE_DIFFS
    static bool try_add_player_to_any_game(player* player, game*& game_instance_ptr, std::string& err);
    static bool try_add_player(player* player, game*& game_instance_ptr, std::string& err);

    static bool try_remove_player(player* player, const std::string& game_id, std::string& err);
    static bool try_remove_player(player* player, game*& game_instance_ptr, std::string& err);
#else
    static bool try_add_player_to_any_game(player* player, game*& game_instance_ptr, object_diff& game_state_diff, std::string& err);
    static bool try_add_player(player* player, game*& game_instance_ptr, object_diff& game_state_diff, std::string& err);

    static bool try_remove_player(player* player, const std::string& game_id, object_diff& game_state_diff, std::string& err);
    static bool try_remove_player(player* player, game*& game_instance_ptr, object_diff& game_state_diff, std::string& err);
#endif
};


#endif //LAMA_GAME_INSTANCE_MANAGER_H
