//
// Created by Manuel on 27.01.2021.
//
#include "game_state.h"

#include <iostream>
#include <utility>

#include "../exceptions/TichuException.h"


game_state::game_state() :
        _id(UUID::create()),
        _draw_pile(draw_pile()), _discard_pile(discard_pile()), _players(std::vector<player>()),
        _is_started(false), _is_finished(false), _current_player_idx(0), _play_direction(1),
        _round_number(0), _starting_player_idx(0)
{ }

game_state::game_state(UUID id) : _id(std::move(id)),
                                  _draw_pile(draw_pile()), _discard_pile(discard_pile()), _players(std::vector<player>()),
                                  _is_started(false), _is_finished(false), _current_player_idx(0), _play_direction(1),
                                  _round_number(0), _starting_player_idx(0)
{ }

game_state::game_state(UUID id, draw_pile draw_pile, discard_pile discard_pile,
                       std::vector<player> &players, bool is_started,
                       bool is_finished, int current_player_idx,
                       int play_direction, int round_number, int starting_player_idx)
        : _id(std::move(id)),
          _draw_pile(std::move(draw_pile)),
          _discard_pile(std::move(discard_pile)),
          _players(players),
          _is_started(is_started),
          _is_finished(is_finished),
          _current_player_idx(current_player_idx),
          _play_direction(play_direction),
          _round_number(round_number),
          _starting_player_idx(starting_player_idx)
{ }

// accessors
std::optional<player> game_state::get_current_player() const {
    if(_players.empty()) {
        return {};
    }
    return _players[_current_player_idx];
}

int game_state::get_player_index(const player &player) const {
    auto it = std::find(_players.begin(), _players.end(), player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

bool game_state::is_player_in_game(const player &player) const {
    return std::find(_players.begin(), _players.end(), player) < _players.end();
}

bool game_state::is_allowed_to_play_now(const player &player) const {
    auto current = get_current_player();
    if (!current) { // no current player
        return false;
    }
    return !player.has_folded() && player == current.value();
}

const std::vector<player>& game_state::get_players() const {
    return _players;
}


#ifdef TICHU_SERVER

// state modification functions without diff
void game_state::setup_round(std::string &err) {

    // update round number
    _round_number += 1;

    // setup draw_pile
    _draw_pile.setup_game(err);

    // setup discard_pile
    _discard_pile.setup_game(err);

    // setup players
    for (int i = 0; i < _players.size(); i++) {
        _players[i].setup_round(err);
        // draw 14 cards
        //card* drawn_card = nullptr;
        for (int j = 0; j < 14; j++) {
            auto drawn_card = _draw_pile.draw(_players[i], err);
            if (!drawn_card) {
                std::cerr << err << std::endl;
            }
        }
    }

    // set a first card onto the discard pile
    auto top_card = _draw_pile.remove_top(err);
    if (top_card) {
        _discard_pile.try_play(top_card.value(), err);
    }
}

void game_state::wrap_up_round(std::string& err) {
    bool is_game_over = false;
    for(int i = 0; i < _players.size(); i++) {
        _players[i].wrap_up_round(err);
        if (_players[i].get_score() >= 40) {
            // The game ends when the first player reaches 40 points
            is_game_over = true;
        }
    }

    if (is_game_over) {
        _is_finished = true;
    } else {
        // decide which player starts in the next round
        _starting_player_idx = (_starting_player_idx + 1) % _players.size();
        // start next round
        setup_round(err);
    }
}

void game_state::update_current_player(std::string& err) {
    int nof_players = _players.size();
    int current_player_idx = _current_player_idx;
    ++current_player_idx %= nof_players;
    bool round_over = true;
    for (int i = 0; i < nof_players; i++) {
        if (!_players[current_player_idx].has_folded()) {
            _current_player_idx = current_player_idx;
            round_over = false;
            break;
        } else {
            ++current_player_idx %= nof_players;
        }
    }

    if (round_over) {
        // all players have folded and the round is over
        wrap_up_round(err);
    }
}

bool game_state::start_game(std::string &err) {
    if (_players.size() < _min_nof_players) {
        err = "You need at least " + std::to_string(_min_nof_players) + " players to start the game.";
        return false;
    }

    if (!_is_started) {
        this->setup_round(err);
        this->_is_started = true;
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        return false;
    }
}

bool game_state::remove_player(const player &player_ptr, std::string &err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx) {
            // reduce current_player_idx if the player who left had a lower index
            _current_player_idx -= 1;
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}


bool game_state::add_player(const player &player_ptr, std::string& err) {
    if (_is_started) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_finished) {
        err = "Could not join game, because the requested game is already finished.";
        return false;
    }
    if (_players.size() >= _max_nof_players) {
        err = "Could not join game, because the max number of players is already reached.";
        return false;
    }
    if (std::find(_players.begin(), _players.end(), player_ptr) != _players.end()) {
        err = "Could not join game, because this player is already subscribed to this game.";
        return false;
    }

    _players.push_back(player_ptr);
    return true;
}


bool game_state::play_card(player &player, const card& card_id, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished) {
        err = "Could not play card, because the requested game is already finished.";
        return false;
    }

    if (_discard_pile.try_play(card_id, player, err)) {
        if (player.get_nof_cards() == 0) {
            // end of game. Calculate scores. Prepare new round
            wrap_up_round(err);
        } else {
            update_current_player(err);
        }
        return true;
    } else {
        return false;
    }
}

bool game_state::fold(player &player, std::string &err) {
    if(!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished) {
        err = "Could not fold, because the requested game is already finished.";
        return false;
    }

    if (player.fold(err)) {
        // Allow next player to play
        update_current_player(err);
        return true;
    } else {
        return false;
    }
}

#endif


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {

    bool_into_json("is_finished", _is_finished, json, alloc);
    bool_into_json("is_started", _is_started, json, alloc);
    int_into_json("current_player_idx", _current_player_idx, json, alloc);
    int_into_json("play_direction", _play_direction, json, alloc);
    int_into_json("starting_player_idx", _starting_player_idx, json, alloc);
    int_into_json("round_number", _round_number, json, alloc);
    string_into_json("id", _id.string(), json, alloc);

    _draw_pile.write_into_json_obj("draw_pile", json, alloc);
    _discard_pile.write_into_json_obj("discard_pile", json, alloc);

    vec_into_json("players", _players, json, alloc);
}


game_state game_state::from_json(const rapidjson::Value &json) {
    //TODO: remove check

        auto players = vec_from_json<player>("players", json);
        auto draw_pile = draw_pile::from_json(json["draw_pile"].GetObject());
        auto discard_pile = discard_pile::from_json(json["discard_pile"].GetObject());

        auto id = string_from_json("id", json);
        auto is_started = bool_from_json("is_started", json);
        auto is_finished = bool_from_json("is_finished", json);
        auto current_player_idx = int_from_json("current_player_idx", json);
        auto play_direction = int_from_json("play_direction", json);
        auto round_number = int_from_json("round_number", json);
        auto starting_player_idx = int_from_json("starting_player_idx", json);

        if (id && players && is_started && is_finished && current_player_idx && play_direction && round_number && starting_player_idx) {
            return game_state {
                    UUID(id.value()),
                    draw_pile,
                    discard_pile,
                    players.value(),
                    is_started.value(),
                    is_finished.value(),
                    current_player_idx.value(),
                    play_direction.value(),
                    round_number.value(),
                    starting_player_idx.value()
            };

        } else {
            throw TichuException("Failed to deserialize game_state. Required entries were missing.");
        }
}

