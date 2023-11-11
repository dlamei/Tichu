// The player_manager only exists on the server side. It stores all connected users since starting the server. It offers
// functionality to retrieve players by id or adding players when they first connect to the server.
//

#ifndef TICHU_PLAYER_MANAGER_H
#define TICHU_PLAYER_MANAGER_H

#include <string>
#include <shared_mutex>
#include <unordered_map>
#include <memory>

#include "../common/game_state/player/player.h"


class player_manager {

private:

    inline static std::shared_mutex _rw_lock;
     static std::unordered_map<UUID, player_ptr> _players_lut;

public:
    //static bool try_get_player(const std::string& _player_id, const player &player_ptr);
    static std::optional<player_ptr> try_get_player(const UUID& player_id);
    static player_ptr add_or_get_player(const std::string& name, const UUID& player_id);
    static std::optional<player_ptr> remove_player(const UUID& player_id);  // not implemented
};


#endif //TICHU_PLAYER_MANAGER_H
