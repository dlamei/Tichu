//
// Created by Manuel on 25.01.2021.
//

#include "game_instance.h"

#include "server_network_manager.h"
#ifndef USE_DIFFS    // NOT USE_DIFFS
#include "../network/responses/full_state_response.h"
#else   // USE_DIFFS
#include "network/responses/state_diff_response.h"
#endif

game_instance::game_instance() {
    _game_state = new game_state();
}

game_state *game_instance::get_game_state() {
    return _game_state;
}

std::string game_instance::get_id() {
    return _game_state->get_id();
}

bool game_instance::is_player_allowed_to_play(player *player) {
    return _game_state->is_allowed_to_play_now(player);
}

bool game_instance::is_full() {
    return _game_state->is_full();
}

bool game_instance::is_started() {
    return _game_state->is_started();
}

bool game_instance::is_finished() {
    return _game_state->is_finished();
}

#ifdef LAMA_SERVER
#ifndef USE_DIFFS
bool game_instance::play_card(player *player, const std::string& card_id, std::string& err) {
    modification_lock.lock();
    if (_game_state->play_card(player, card_id, err)) {
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::draw_card(player *player, card*& drawn_card, std::string& err) {
    modification_lock.lock();
    if (_game_state->draw_card(player, err)) {
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;

}

bool game_instance::fold(player *player, std::string& err) {
    modification_lock.lock();
    if (_game_state->fold(player, err)) {
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::start_game(player* player, std::string &err) {
    modification_lock.lock();
    if (_game_state->start_game(err)) {
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_remove_player(player *player, std::string &err) {
    modification_lock.lock();
    if (_game_state->remove_player(player, err)) {
        player->set_game_id("");
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player *new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state->add_player(new_player, err)) {
        new_player->set_game_id(get_id());
        // send state update to all other players
        full_state_response state_update_msg = full_state_response(this->get_id(), *_game_state);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

#else // USE_DIFFS

bool game_instance::play_card(player *player, const std::string& card_id, object_diff& game_state_diff, std::string& err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->play_card(player, card_id, game_state_diff, err)) {
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::draw_card(player *player, card*& drawn_card, object_diff& game_state_diff, std::string& err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->draw_card(player, game_state_diff, err)) {
        // send state update to all other players
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;

}

bool game_instance::fold(player *player, object_diff& game_state_diff, std::string& err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->fold(player, game_state_diff, err)) {
        // send state update to all other players
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::start_game(player* player, object_diff& game_state_diff, std::string &err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->start_game(game_state_diff, err)) {
        // send state update to all other players
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_remove_player(player *player, object_diff& game_state_diff, std::string &err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->remove_player(player, game_state_diff, err)) {
        player->set_game_id("");
        // send state update to all other players
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool game_instance::try_add_player(player *new_player, object_diff& game_state_diff, std::string &err) {
    game_state_diff = object_diff(get_id(), _game_state->get_name());
    modification_lock.lock();
    if (_game_state->add_player(new_player, game_state_diff, err)) {
        new_player->set_game_id(get_id());
        // send state update to all other players
        state_diff_response state_update_msg = state_diff_response(this->get_id(), game_state_diff);
        server_network_manager::broadcast_message(state_update_msg, _game_state->get_players(), new_player);
        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}
#endif
#endif
