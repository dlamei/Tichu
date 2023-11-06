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

void broadcast_full_state_response(const game_state &state, const player &player) {
    auto update_msg = full_state_response{ state.to_json() };
    auto resp = server_msg(state.get_id(), update_msg);
    server_network_manager::broadcast_message(resp, state.get_players(), player);
}

bool game_instance::play_card(player &player, const card& card_id, std::string& err) {
    modification_lock.lock();
    if (_game_state.play_card(player, card_id, err)) {
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::draw_card(player &player, std::string& err) {
    modification_lock.lock();
    auto drawn_card = _game_state.draw_card(player, err);
    if (drawn_card) {
        broadcast_full_state_response(_game_state, player);
    }
    modification_lock.unlock();
    return drawn_card;

}

bool game_instance::fold(player &player, std::string& err) {
    modification_lock.lock();
    if (_game_state.fold(player, err)) {
        // send state update to all other players
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::start_game(player &player, std::string &err) {
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

bool game_instance::try_remove_player(player &player, std::string &err) {
    modification_lock.lock();
    if (_game_state.remove_player(player, err)) {
        player.set_game_id(UUID(""));
        // send state update to all other players
        broadcast_full_state_response(_game_state, player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player &new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state.add_player(new_player, err)) {
        new_player.set_game_id(get_id());
        // send state update to all other players
        broadcast_full_state_response(_game_state, new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

