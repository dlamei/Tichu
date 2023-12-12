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
    GamePhase game_phase = _game_state.get_game_phase();
    return (game_phase == GamePhase::INROUND) || (game_phase == GamePhase::PREROUND) || (game_phase == GamePhase::SWAPPING);
    
}

bool GameInstance::is_finished() {
    return _game_state.get_game_phase() == GamePhase::POSTGAME;
}


void send_full_state_response(const Player &recipient, const GameState &state, const std::vector<Event> events) {
    auto update_msg = full_state_response{ state, events };
    auto resp = ServerMsg(update_msg);
    server_network_manager::broadcast_single_message(resp, state.get_players(), recipient);
}


bool GameInstance::play_combi(const player_ptr& player, CardCombination &combi, std::string &err) {
    modification_lock.lock();
    if (_game_state.play_combi(*player, combi, err)) {
        for(auto recipient : _game_state.get_players()){
            if(*recipient != *player){
            Event event{EventType::PLAY_COMBI, player->get_player_name(), {}, {}, {}};
            send_full_state_response(*recipient, _game_state, {event});
            }
        }
        Event event{EventType::PLAY_COMBI, {}, {}, {}, {}};
        send_full_state_response(*player, _game_state, {event});

        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool GameInstance::call_grand_tichu(const player_ptr &player, Tichu tichu, std::string &err) {
    modification_lock.lock();
    if (_game_state.call_grand_tichu(*player, tichu, err)) {
        // send state update to all players
        for(auto recipient : _game_state.get_players()){
            if(*recipient != *player) {
                Event event{EventType::GRAND_TICHU, player->get_player_name(), {}, {}, {}};
                send_full_state_response(*recipient, _game_state, {event});
            }
        }
        Event event{EventType::GRAND_TICHU, {}, {}, {}, {}};
        send_full_state_response(*player, _game_state, {event});

        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool GameInstance::call_small_tichu(const player_ptr &player, Tichu tichu, std::string &err) {
    modification_lock.lock();
    if (_game_state.call_small_tichu(*player, tichu, err)) {
        // send state update to all players
        for(auto recipient : _game_state.get_players()){
            if(*recipient != *player) {
                Event event{EventType::SMALL_TICHU, player->get_player_name(), {}, {}, {}};
                send_full_state_response(*recipient, _game_state, {event});
            }
        }
        Event event{EventType::SMALL_TICHU, {}, {}, {}, {}};
        send_full_state_response(*player, _game_state, {event});

        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool GameInstance::start_game(player_ptr player, std::string &err) {
    modification_lock.lock();
    if (_game_state.start_game(err)) {
        // send state update to all players
        for(auto recipient : _game_state.get_players()){
            Event event{EventType::GAME_START, {}, {}, {}, {}};
            send_full_state_response(*recipient, _game_state, {event});
        }

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
        for(auto recipient : _game_state.get_players()){
            if(*recipient != *player) {
                Event event{EventType::PLAYER_JOINED, player->get_player_name(), {}, {}, {}};
                send_full_state_response(*recipient, _game_state, {event});
            }
        }
        return true;
    }
    modification_lock.unlock();
    return false;
}

bool GameInstance::try_add_player(player_ptr new_player, std::string &err) {
    modification_lock.lock();
    if (_game_state.add_player(new_player, err)) {
        // send state update to all players
        for(auto recipient : _game_state.get_players()){
            if(*recipient != *new_player) {
                Event event{EventType::PLAYER_JOINED, new_player->get_player_name(), {}, {}, {}};
                send_full_state_response(*recipient, _game_state, {event});
            }
        }
        Event event{EventType::PLAYER_JOINED, {}, {}, {}, {}};
        send_full_state_response(*new_player, _game_state, {event});

        modification_lock.unlock();
        return true;
    }
    modification_lock.unlock();
    return false;
}

