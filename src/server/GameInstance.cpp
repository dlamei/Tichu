#include "GameInstance.h"

#include <utility>
#include "server_network_manager.h"


GameInstance::GameInstance()
        : _game_state() {
}

const GameState &GameInstance::get_game_state() {
    return _game_state;
}

bool GameInstance::is_player_allowed_to_play(const Player &player) {
    return _game_state.is_allowed_to_play_now(player);
}

bool GameInstance::is_full() {
    return _game_state.is_full();
}

bool GameInstance::is_started() {
    return _game_state.is_started();
}

bool GameInstance::is_finished() {
    return _game_state.is_game_finished();
}

void broadcast_full_state_response(const GameState &state, player_ptr player) {
    auto update_msg = full_state_response{ state };
    auto resp = ServerMsg(state.get_id(), update_msg);
    server_network_manager::broadcast_message(resp, state.get_players(), std::move(player));
}

bool GameInstance::play_combi(const player_ptr& player, CardCombination &combi, std::string &err) {
    modification_lock.lock();
    if (_game_state.play_combi(*player, combi, err)) {
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}


bool GameInstance::start_game(player_ptr player, std::string &err) {
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

bool GameInstance::try_remove_player(player_ptr player, std::string &err) {
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

bool GameInstance::try_add_player(player_ptr new_player, std::string &err) {
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

