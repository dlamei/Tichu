// The game_instance_manager only exists on the server side. It stores all currently active games and offers
// functionality to retrieve game instances by id and adding players to games.
// If a new player requests to join a game but no valid game_instance is available, then this class
// will generate a new game_instance and add it to the unordered_map of (active) game instances.

#include "game_instance_manager.h"

#include "player_manager.h"
#include "server_network_manager.h"

// Initialize static map
std::unordered_map<UUID, std::shared_ptr<game_instance>> game_instance_manager::games_lut = {};

game_instance_ptr game_instance_manager::find_joinable_game_instance() {
    std::vector<UUID> to_remove;
    std::optional<game_instance_ptr> res = {};
    games_lut_lock.lock_shared();
    for (auto it = games_lut.begin(); it != games_lut.end(); it++) {
        if (it->second->is_finished()) {    // also check if there are any finished games that can be removed
            to_remove.push_back(it->first);
        } else if (!it->second->is_full() && !it->second->is_started()) {
            res = it->second; // found a non-full, non-started game
            break;
        }
    }
    games_lut_lock.unlock_shared();

    if (!res) {
        // couldn't find a non-full, non-started game -> create a new one
        res = create_new_game();
    }

    // remove all finished games
    if (!to_remove.empty()) {
        games_lut_lock.lock();
        for (auto& game_id : to_remove) {
            games_lut.erase(game_id);
        }
        games_lut_lock.unlock();
    }
    return res.value();
}

std::shared_ptr<game_instance> game_instance_manager::create_new_game() {
    std::shared_ptr<game_instance> new_game = std::make_shared<game_instance>(game_instance());
    games_lut_lock.lock();  // exclusive
    game_instance_manager::games_lut.insert({new_game->get_id(), new_game});
    games_lut_lock.unlock();
    return new_game;
}


std::optional<game_instance_ptr> game_instance_manager::try_get_game_instance(const UUID &game_id) {
    std::optional<game_instance_ptr> game_instance_ptr = {};
    games_lut_lock.lock_shared();
    auto it = game_instance_manager::games_lut.find(game_id);
    if (it != games_lut.end()) {
        game_instance_ptr = it->second;
    }
    games_lut_lock.unlock_shared();
    return game_instance_ptr;
}

std::optional<std::tuple<player_ptr, game_instance_ptr>>
game_instance_manager::try_get_player_and_game_instance(const UUID& player_id, std::string& err) {
    auto player = player_manager::try_get_player(player_id);
    if (player) {
        auto game_instance_ptr = game_instance_manager::try_get_game_instance(player.value()->get_game_id());
        if (game_instance_ptr) {
            return std::tuple {player.value(), game_instance_ptr.value()};
        } else {
            err = "Could not find _game_id" + player.value()->get_game_id().string() + " associated with this player";
        }
    } else {
        err = "Could not find requested player " + player_id.string() + " in database.";
    }
    return {};
}


std::optional<game_instance_ptr> game_instance_manager::try_add_player_to_any_game(player &player, std::string& err) {

    // check that player is not already subscribed to another game
    if (player.get_game_id().empty()) {
        err = "Could not join game. Player is already active in a game";
        return {};
    }

        // Join any non-full, non-started game
        for (int i = 0; i < 10; i++) {
            // make at most 10 attempts of joining a src (due to concurrency, the game could already be full or started by the time
            // try_add_player_to_any_game() is invoked) But with only few concurrent requests it should succeed in the first iteration.
            auto game_instance_ptr = find_joinable_game_instance();
            if (try_add_player(player, *game_instance_ptr, err)) {
                return game_instance_ptr;
            }
        }
        return {};
    //TODO: not any game?
}


bool game_instance_manager::try_add_player(player &player, game_instance &game_instance_ptr, std::string& err) {
    if (player.get_game_id().empty()) {
        if (player.get_game_id() != game_instance_ptr.get_id()) {
            err = "Player is already active in a different src with id " + player.get_game_id().string();
        } else {
            err = "Player is already active in this src";
        }
        return false;
    }

    if (game_instance_ptr.try_add_player(player, err)) {
        player.set_game_id(game_instance_ptr.get_id());   // mark that this player is playing in a src
        return true;
    } else {
        return false;
    }
}

bool game_instance_manager::try_remove_player(player &player, const UUID &game_id, std::string &err) {
    auto game_instance = try_get_game_instance(game_id);
    if (game_instance) {
        return try_remove_player(player, *game_instance.value(), err);
    } else {
        err = "The requested src could not be found. Requested src id was " + game_id.string();
        return false;
    }
}

bool game_instance_manager::try_remove_player(player &player, game_instance &game_instance_ptr, std::string &err) {
    return game_instance_ptr.try_remove_player(player, err);
}

