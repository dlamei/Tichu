#include "game_instance.h"
#include "server_network_manager.h"


game_instance::game_instance()
: _game_state()
{
}

const game_state &game_instance::get_game_state() {
    return _game_state;
}

bool game_instance::is_player_allowed_to_play(const player &player) {
    return _game_state.is_allowed_to_play_now(player);
}

bool game_instance::is_full() {
    return _game_state.is_full();
}

bool game_instance::is_started() {
    return _game_state.is_started();
}

bool game_instance::is_finished() {
    return _game_state.is_finished();
}

void broadcast_full_state_response(const game_state &state, player_ptr player) {
    auto update_msg = full_state_response{ state.to_json() };
    auto resp = server_msg(state.get_id(), update_msg);
    server_network_manager::broadcast_message(resp, state.get_players(), player);
}

bool game_instance::play_combi(player_ptr player, const card_combination& combi, std::string& err) {
    modification_lock.lock();
    if (_game_state.play_combi(*player, combi, err)) {
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}


bool game_instance::start_game(player_ptr player, std::string &err) {
    modification_lock.lock();
    if (_game_state.start_game(err)) {
        // send state update to all other players
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_remove_player(player_ptr player, std::string &err) {
    modification_lock.lock();
    if (_game_state.remove_player(player, err)) {
        player->set_game_id(UUID(""));
        // send state update to all other players
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player_ptr new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state.add_player(new_player, err)) {
        new_player->set_game_id(get_id());
        // send state update to all other players
        broadcast_full_state_response(_game_state, new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

