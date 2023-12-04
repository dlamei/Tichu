// The player_manager only exists on the server side. It stores all connected users since starting the server. It offers
// functionality to retrieve players by id or adding players when they first connect to the server.
//

#include "player_manager.h"

// Initialize static map
std::unordered_map<UUID, std::shared_ptr<Player>> player_manager::_players_lut = {};

std::optional<player_ptr> player_manager::try_get_player(const UUID &player_id) {
    _rw_lock.lock_shared();
    std::optional<player_ptr> player = {};
    auto it = player_manager::_players_lut.find(player_id);
    if (it != _players_lut.end()) {
        //player_ptr = it->second;
        player = it->second;
    }
    _rw_lock.unlock_shared();
    return player;
}

player_ptr player_manager::add_or_get_player(const std::string &name, const UUID &player_id) {
    auto player_ptr = try_get_player(player_id);

    if (player_ptr) {
        return player_ptr.value();
    }
    auto team = Team::A;
    auto player_val = std::make_shared<Player>(Player(player_id, name, team));
    _rw_lock.lock();    // exclusive
    player_manager::_players_lut.insert({player_id, player_val});
    _rw_lock.unlock();
    return player_val;
}

std::optional<player_ptr> player_manager::remove_player(const UUID &player_id) {
    auto player = try_get_player(player_id);
    if (player) {
        _rw_lock.lock();    // exclusive
        int nof_removals = player_manager::_players_lut.erase(player_id);
        _rw_lock.unlock();
        return player;
    }
    return {};
}

