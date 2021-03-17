//
// Created by Manuel on 27.01.2021.
//

#include "game_state.h"

#include <algorithm>
#include <iostream>

#include "../common/utils/LamaException.h"
#include "../reactive_state/array_helpers.h"
#include "../reactive_state/diffs/array_diff.h"
#include "../reactive_state/diffs/object_diff.h"



game_state::game_state() : reactive_object("game_state") {
    this->_draw_pile = new draw_pile();
    this->_discard_pile = new discard_pile();
    this->_players = std::vector<player*>();
    this->_is_started = new reactive_value<bool>("is_started", false);
    this->_is_finished = new reactive_value<bool>("is_finished", false);
    this->_current_player_idx = new reactive_value<int>("current_player_idx", 0);
    this->_play_direction = new reactive_value<int>("play_direction", 1);
    this->_round_number = new reactive_value<int>("round_number", 0);
    this->_starting_player_idx = new reactive_value<int>("starting_player_idx", 0);
}

game_state::game_state(base_params params, draw_pile *draw_pile, discard_pile *discard_pile,
                       std::vector<player *> &players, reactive_value<bool> *is_started,
                       reactive_value<bool> *is_finished, reactive_value<int> *current_player_idx,
                       reactive_value<int> *play_direction, reactive_value<int>* round_number, reactive_value<int> *starting_player_idx)
        : reactive_object(params),
          _draw_pile(draw_pile),
          _discard_pile(discard_pile),
          _players(players),
          _is_started(is_started),
          _is_finished(is_finished),
          _current_player_idx(current_player_idx),
          _play_direction(play_direction),
          _round_number(round_number),
          _starting_player_idx(starting_player_idx)
{ }

game_state::game_state(base_params params) : reactive_object(params) {
    this->_draw_pile = new draw_pile();
    this->_discard_pile = new discard_pile();
    this->_players = std::vector<player*>();
    this->_is_started = new reactive_value<bool>("is_started", false);
    this->_is_finished = new reactive_value<bool>("is_finished", false);
    this->_current_player_idx = new reactive_value<int>("current_player_idx", 0);
    this->_play_direction = new reactive_value<int>("play_direction", 1);
    this->_round_number = new reactive_value<int>("round_number", 0);
    this->_starting_player_idx = new reactive_value<int>("starting_player_idx", 0);
}

game_state::~game_state() {
    if (_is_started != nullptr) {
        delete _is_started;
        delete _is_finished;
        delete _draw_pile;
        delete _discard_pile;
        delete _current_player_idx;
        delete _starting_player_idx;
        delete _play_direction;
        delete _round_number;

        _is_started = nullptr;
        _is_finished = nullptr;
        _draw_pile = nullptr;
        _discard_pile = nullptr;
        _current_player_idx = nullptr;
        _starting_player_idx = nullptr;
        _play_direction = nullptr;
        _round_number = nullptr;
    }
}

// accessors
player* game_state::get_current_player() const {
    return _players[_current_player_idx->get_value()];
}

discard_pile *game_state::get_discard_pile() const {
    return _discard_pile;
}

draw_pile *game_state::get_draw_pile() const {
    return _draw_pile;
}

bool game_state::is_full() const {
    return _players.size() == _max_nof_players;
}

bool game_state::is_started() const {
    return _is_started->get_value();
}

bool game_state::is_finished() const {
    return _is_finished->get_value();
}

int game_state::get_round_number() const {
    return _round_number->get_value();
}

int game_state::get_player_index(player *player) const {
    auto it = std::find(_players.begin(), _players.end(), player);
    if (it == _players.end()) {
        return -1;
    } else {
        return it - _players.begin();
    }
}

bool game_state::is_player_in_game(player *player) const {
    return std::find(_players.begin(), _players.end(), player) < _players.end();
}

bool game_state::is_allowed_to_play_now(player *player) const {
    return !player->has_folded() && player == get_current_player();
}

std::vector<player*>& game_state::get_players() {
    return _players;
}


#ifdef LAMA_SERVER

#ifndef USE_DIFFS // Full state version

// state modification functions without diff
void game_state::setup_round(std::string &err) {

    // update round number
    _round_number->set_value(_round_number->get_value() + 1);

    // setup draw_pile
    _draw_pile->setup_game(err);

    // setup discard_pile
    _discard_pile->setup_game(err);

    // setup players
    for (int i = 0; i < _players.size(); i++) {
        _players[i]->setup_round(err);
        // draw 6 cards
        card* drawn_card = nullptr;
        for (int j = 0; j < 6; j++) {
            if (!_draw_pile->draw(_players[i], drawn_card, err)) {
                std::cerr << err << std::endl;
            }
        }
    }
}

void game_state::wrap_up_round(std::string& err) {
    bool is_game_over = false;
    for(int i = 0; i < _players.size(); i++) {
        _players[i]->wrap_up_round(err);
        if (_players[i]->get_score() >= 40) {
            // The game ends when the first player reaches 40 points
            is_game_over = true;
        }
    }

    if (is_game_over) {
        this->_is_finished->set_value(true);
    } else {
        // decide which player starts in the next round
        _starting_player_idx->set_value((_starting_player_idx->get_value() + 1) % _players.size());
        // start next round
        setup_round(err);
    }
}

void game_state::update_current_player(std::string& err) {
    int nof_players = _players.size();
    int current_player_idx = _current_player_idx->get_value();
    ++current_player_idx %= nof_players;
    bool round_over = true;
    for (int i = 0; i < nof_players; i++) {
        if (_players[current_player_idx]->has_folded() == false) {
            _current_player_idx->set_value(current_player_idx);
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
        err = "Need at least " + std::to_string(_min_nof_players) + " players to start the game.";
        return false;
    }

    if (!_is_started->get_value()) {
        this->setup_round(err);
        this->_is_started->set_value(true);
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        return false;
    }
}

bool game_state::remove_player(player *player_ptr, std::string &err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx->get_value()) {
            // reduce current_player_idx if the player who left had a lower index
            _current_player_idx->set_value(_current_player_idx->get_value() - 1);
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}


bool game_state::add_player(player* player_ptr, std::string& err) {
    if (_is_started->get_value()) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_finished->get_value()) {
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

bool game_state::draw_card(player *player, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_draw_pile->is_empty()) {
        err = "Draw pile is empty. Cannot draw a card.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not draw card, because the requested game is already finished.";
        return false;
    }

    card* drawn_card;
    if (_draw_pile->draw(player, drawn_card, err)) {
        update_current_player(err); // next players turn
        return true;
    } else {
        return false;
    }
}

bool game_state::play_card(player *player, const std::string& card_id, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not play card, because the requested game is already finished.";
        return false;
    }

    if (_discard_pile->try_play(card_id, player, err)) {
        if (player->get_nof_cards() == 0) {
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

bool game_state::fold(player *player, std::string &err) {
    if(!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not fold, because the requested game is already finished.";
        return false;
    }

    if (player->fold(err)) {
        // Allow next player to play
        update_current_player(err);
        return true;
    } else {
        return false;
    }
}


#else // USE_DIFFS
// with state_diff
void game_state::setup_round(object_diff &game_state_diff, std::string &err) {

    // update round number
    value_diff<int>* round_number_diff = nullptr;
    _round_number->set_value(_round_number->get_value() + 1, round_number_diff);
    game_state_diff.add_param_diff(round_number_diff->get_name(), round_number_diff);

    // setup draw_pile
    object_diff* draw_pile_diff = new object_diff(_draw_pile->get_id(), _draw_pile->get_name());
    _draw_pile->setup_game(*draw_pile_diff, err);
    game_state_diff.add_param_diff(draw_pile_diff->get_name(), draw_pile_diff);

    // setup discard_pile
    object_diff* discard_pile_diff = new object_diff(_discard_pile->get_id(), _discard_pile->get_name());
    _discard_pile->setup_game(*discard_pile_diff, err);
    game_state_diff.add_param_diff(discard_pile_diff->get_name(), discard_pile_diff);

    // setup players
    array_diff* players_diff = new array_diff(this->_id + "_players", "players");
    for (int i = 0; i < _players.size(); i++) {
        object_diff* player_diff = new object_diff(_players[i]->get_id(), "player");
        _players[i]->setup_round(*player_diff, err);
        // draw 6 cards
        card* drawn_card = nullptr;
        for (int j = 0; j < 6; j++) {
            if (!_draw_pile->draw(_players[i], drawn_card, *player_diff, *draw_pile_diff, err)) {
                std::cerr << err << std::endl;
            }
        }
        players_diff->add_modification(i, _players[i]->get_id(), player_diff);
    }
    game_state_diff.add_param_diff(players_diff->get_name(), players_diff);
}

void game_state::wrap_up_round(object_diff& game_state_diff, std::string& err) {
    bool is_game_over = false;
    array_diff* players_diff = new array_diff(this->_id + "_players", "players");
    for(int i = 0; i < _players.size(); i++) {
        object_diff* player_diff = new object_diff(_players[i]->get_id(), _players[i]->get_name());
        _players[i]->wrap_up_round(*player_diff, err);
        if (player_diff->has_changes()) {
            players_diff->add_modification(i, _players[i]->get_id(), player_diff);
        }
        if (_players[i]->get_score() >= 40) {
            // The game ends when the first player reaches 40 points
            is_game_over = true;
        }
    }

    if (is_game_over) {
        this->_is_finished->set_value(true);
        game_state_diff.add_param_diff(_is_finished->get_name(), _is_finished->to_full_diff());
    } else {
        // start next round
        // decide which player starts in the next round
        _starting_player_idx->set_value((_starting_player_idx->get_value() + 1) % _players.size());
        game_state_diff.add_param_diff(_starting_player_idx->get_name(), _starting_player_idx->to_full_diff());

        // setup next round
        setup_round(game_state_diff, err);
    }
}

void game_state::update_current_player(object_diff& game_state_diff, std::string& err) {
    int nof_players = _players.size();
    int current_player_idx = _current_player_idx->get_value();
    ++current_player_idx %= nof_players;
    for (int i = 0; i < nof_players; i++) {
        if (_players[current_player_idx]->has_folded() == false) {
            _current_player_idx->set_value(current_player_idx);
            game_state_diff.add_param_diff(_current_player_idx->get_name(), _current_player_idx->to_full_diff());
            return;
        } else {
            ++current_player_idx %= nof_players;
        }
    }

    wrap_up_round(game_state_diff, err);
}


bool game_state::start_game(object_diff &game_state_diff, std::string &err) {
    if (_players.size() < _min_nof_players) {
        err = "Need at least " + std::to_string(_min_nof_players) + " players to start the game.";
        return false;
    }

    if (!_is_started->get_value()) {
        value_diff<bool>* has_started_diff = nullptr;
        this->_is_started->set_value(true, has_started_diff);
        this->setup_round(game_state_diff, err);
        game_state_diff.add_param_diff(has_started_diff->get_name(), has_started_diff);
        return true;
    } else {
        err = "Could not start game, as the game was already started";
        return false;
    }
}

bool game_state::remove_player(player *player_ptr, object_diff& game_state_diff, std::string& err) {
    int idx = get_player_index(player_ptr);
    if (idx != -1) {
        if (idx < _current_player_idx->get_value()) {
            // reduce current_player_idx if the player who left had a lower index
            value_diff<int>* current_player_diff = nullptr;
            _current_player_idx->set_value(_current_player_idx->get_value() - 1, current_player_diff);
            game_state_diff.add_param_diff(current_player_diff->get_name(), current_player_diff);
        }
        _players.erase(_players.begin() + idx);
        array_diff* arr_diff = new array_diff(this->_id + "_players", "players");
        arr_diff->add_removal(idx, player_ptr->get_id());
        game_state_diff.add_param_diff(arr_diff->get_name(), arr_diff);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}

bool game_state::add_player(player* player_ptr, object_diff& game_state_diff, std::string& err) {
    if (_is_started->get_value()) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_finished->get_value()) {
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
    array_diff* arr_diff = new array_diff(this->_id + "_players", "players");
    arr_diff->add_insertion(_players.size() - 1, player_ptr->get_id(), player_ptr->to_full_diff());
    game_state_diff.add_param_diff(arr_diff->get_name(), arr_diff);
    return true;
}

bool game_state::draw_card(player *player, object_diff &game_state_diff, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_draw_pile->is_empty()) {
        err = "Draw pile is empty. Cannot draw a card.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not draw card, because the requested game is already finished.";
        return false;
    }

    card* drawn_card;
    object_diff* pile_diff = new object_diff(_draw_pile->get_id(), "draw_pile");
    object_diff* player_diff = new object_diff(this->_id + "_players", "player");
    if (_draw_pile->draw(player, drawn_card, *player_diff,*pile_diff, err)) {
        game_state_diff.add_param_diff(pile_diff->get_name(), pile_diff);
        // add player_diff to array_diff
        array_diff* arr_diff = new array_diff(this->_id + "_players", "players");
        arr_diff->add_modification(get_player_index(player), player->get_id(), player_diff);
        game_state_diff.add_param_diff(arr_diff->get_name(), arr_diff);

        update_current_player(game_state_diff, err);    // next player's turn
        return true;
    } else {
        delete pile_diff;
        delete player_diff;
        return false;
    }
}

bool game_state::play_card(player *player, const std::string& card_id, object_diff& game_state_diff, std::string &err) {
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not play card, because the requested game is already finished.";
        return false;
    }

    object_diff* pile_diff = new object_diff(_discard_pile->get_id(), "discard_pile");
    object_diff* player_diff = new object_diff(player->get_id(), "player");
    if (_discard_pile->try_play(card_id, player, *pile_diff, *player_diff, err)) {
        game_state_diff.add_param_diff(pile_diff->get_name(), pile_diff);
        // put player_diff into array_diff
        array_diff* players_diff = new array_diff(this->_id + "_players", "players");
        players_diff->add_modification(get_player_index(player), player->get_id(), player_diff);
        game_state_diff.add_param_diff(players_diff->get_name(), players_diff);

        if (player->get_nof_cards() == 0) {
            // end of game. Calculate scores. Prepare new round
            wrap_up_round(game_state_diff, err);
        } else {
            update_current_player(game_state_diff, err);
        }
        return true;
    } else {
        delete pile_diff;
        delete player_diff;
        return false;
    }
}

bool game_state::fold(player *player, object_diff& game_state_diff, std::string &err) {
    if(!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_finished->get_value()) {
        err = "Could not fold, because the requested game is already finished.";
        return false;
    }

    object_diff* player_diff = new object_diff(player->get_id(), "player");
    if (player->fold(*player_diff, err)) {
        array_diff* players_diff = new array_diff(this->_id + "_players", "players");
        players_diff->add_modification(get_player_index(player), player->get_id(), player_diff);
        game_state_diff.add_param_diff(players_diff->get_name(), players_diff);

        // Allow next player to play
        update_current_player(game_state_diff, err);

        return true;
    } else {
        delete player_diff;
        return false;
    }
}

#endif
#endif

// reactive_object interface
bool game_state::apply_diff_specialized(const diff* state_diff) {
    const object_diff* valid_diff = dynamic_cast<const object_diff*>(state_diff);
    if (valid_diff != nullptr && valid_diff->get_id() == this->_id) {
        if (valid_diff->get_timestamp()->is_newer(this->_timestamp) && valid_diff->has_changes()) {
            bool has_changed = false;
            diff* child_diff = nullptr;
            if (valid_diff->try_get_param_diff(_is_started->get_name(), child_diff)) {
                has_changed |= _is_started->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_is_finished->get_name(), child_diff)) {
                has_changed |= _is_finished->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_current_player_idx->get_name(), child_diff)) {
                has_changed |= _current_player_idx->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_starting_player_idx->get_name(), child_diff)) {
                has_changed |= _starting_player_idx->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_draw_pile->get_name(), child_diff)) {
                has_changed |= _draw_pile->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_discard_pile->get_name(), child_diff)) {
                has_changed |= _discard_pile->apply_diff_specialized(child_diff);
            }
            if (valid_diff->try_get_param_diff(_round_number->get_name(), child_diff)) {
                has_changed |= _round_number->apply_diff_specialized(child_diff);
            }

            if (valid_diff->try_get_param_diff("players", child_diff)) {
                const array_diff* arr_diff = dynamic_cast<const array_diff*>(child_diff);
                has_changed |= array_helpers::apply_diff<player>(_players, arr_diff);
            }

            return has_changed;
            // TODO update timestamp
        }
    }
    return false;
}

diff *game_state::to_full_diff() const {
    object_diff* game_state_diff = new object_diff(this->_id, this->_name);

    game_state_diff->add_param_diff("is_finished", _is_finished->to_full_diff());
    game_state_diff->add_param_diff("is_started", _is_started->to_full_diff());
    game_state_diff->add_param_diff("current_player_idx", _current_player_idx->to_full_diff());
    game_state_diff->add_param_diff("play_direction", _play_direction->to_full_diff());
    game_state_diff->add_param_diff("starting_player_idx", _starting_player_idx->to_full_diff());
    game_state_diff->add_param_diff("round_number", _round_number->to_full_diff());
    game_state_diff->add_param_diff("draw_pile", _draw_pile->to_full_diff());
    game_state_diff->add_param_diff("discard_pile", _discard_pile->to_full_diff());

    array_diff* players_diff = new array_diff(this->_id + "_players", "players");
    for (int i = 0; i < _players.size(); i++) {
        players_diff->add_insertion(i, _players[i]->get_id(), _players[i]->to_full_diff());
    }
    game_state_diff->add_param_diff("players", players_diff);

    return game_state_diff;
}


game_state *game_state::from_diff(const diff* full_game_state_diff) {
    const object_diff* full_diff = dynamic_cast<const object_diff*>(full_game_state_diff);
    if (full_diff != nullptr && full_diff->get_name() == "game_state") {

        diff* draw_pile_diff = nullptr;
        diff* discard_pile_diff = nullptr;
        diff* is_finished_diff = nullptr;
        diff* is_started_diff = nullptr;
        diff* current_player_idx_diff = nullptr;
        diff* play_direction_diff = nullptr;
        diff* starting_player_idx_diff = nullptr;
        diff* round_number_diff = nullptr;
        diff* players_diff = nullptr;

        if (!full_diff->try_get_param_diff("draw_pile", draw_pile_diff)) {
            std::cerr << "Failed to create from diff. 'draw_pile' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("discard_pile", discard_pile_diff)) {
            std::cerr << "Failed to create from diff. 'draw_pile' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("is_finished", is_finished_diff)) {
            std::cerr << "Failed to create from diff. 'is_finished' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("is_started", is_started_diff)) {
            std::cerr << "Failed to create from diff. 'is_started' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("current_player_idx", current_player_idx_diff)) {
            std::cerr << "Failed to create from diff. 'current_player_idx' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("play_direction", play_direction_diff)) {
            std::cerr << "Failed to create from diff. 'play_direction' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("starting_player_idx", starting_player_idx_diff)) {
            std::cerr << "Failed to create from diff. 'starting_player_idx' was missing!" << std::endl;
        }

        if (!full_diff->try_get_param_diff("round_number", round_number_diff)) {
            std::cerr << "Failed to create from diff. 'round_number' was missing!" << std::endl;
        }

        std::vector<player*> players;
        if (full_diff->try_get_param_diff("players", players_diff)) {
            const array_diff* arr_diff = dynamic_cast<const array_diff*>(players_diff);
            players = array_helpers::vector_from_diff<player>(arr_diff);
        }

        return new game_state(
                reactive_object::extract_base_params(*full_diff),
                draw_pile::from_diff(dynamic_cast<const object_diff*>(draw_pile_diff)),
                discard_pile::from_diff(dynamic_cast<const object_diff*>(discard_pile_diff)),
                players,
                reactive_value<bool>::from_diff(is_started_diff),
                reactive_value<bool>::from_diff(is_finished_diff),
                reactive_value<int>::from_diff(current_player_idx_diff),
                reactive_value<int>::from_diff(play_direction_diff),
                reactive_value<int>::from_diff(round_number_diff),
                reactive_value<int>::from_diff(starting_player_idx_diff)
        );
    } else {
        throw LamaException("Attempt to initialize game_state with a diff of name " + full_diff->get_name());
    }
}


// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    reactive_object::write_into_json(json, allocator);

    rapidjson::Value is_finished_val(rapidjson::kObjectType);
    _is_finished->write_into_json(is_finished_val, allocator);
    json.AddMember("is_finished", is_finished_val, allocator);

    rapidjson::Value is_started_val(rapidjson::kObjectType);
    _is_started->write_into_json(is_started_val, allocator);
    json.AddMember("is_started", is_started_val, allocator);

    rapidjson::Value current_player_idx_val(rapidjson::kObjectType);
    _current_player_idx->write_into_json(current_player_idx_val, allocator);
    json.AddMember("current_player_idx", current_player_idx_val, allocator);

    rapidjson::Value play_direction_val(rapidjson::kObjectType);
    _play_direction->write_into_json(play_direction_val, allocator);
    json.AddMember("play_direction", play_direction_val, allocator);

    rapidjson::Value starting_player_idx_val(rapidjson::kObjectType);
    _starting_player_idx->write_into_json(starting_player_idx_val, allocator);
    json.AddMember("starting_player_idx", starting_player_idx_val, allocator);

    rapidjson::Value round_number_val(rapidjson::kObjectType);
    _round_number->write_into_json(round_number_val, allocator);
    json.AddMember("round_number", round_number_val, allocator);

    rapidjson::Value draw_pile_val(rapidjson::kObjectType);
    _draw_pile->write_into_json(draw_pile_val, allocator);
    json.AddMember("draw_pile", draw_pile_val, allocator);

    rapidjson::Value discard_pile_val(rapidjson::kObjectType);
    _discard_pile->write_into_json(discard_pile_val, allocator);
    json.AddMember("discard_pile", discard_pile_val, allocator);

    json.AddMember("players", diffable_utils::serialize_vector(_players, allocator), allocator);
}


game_state* game_state::from_json(const rapidjson::Value &json) {
    if (json.HasMember("is_finished")
        && json.HasMember("is_started")
        && json.HasMember("current_player_idx")
        && json.HasMember("play_direction")
        && json.HasMember("round_number")
        && json.HasMember("starting_player_idx")
        && json.HasMember("players")
        && json.HasMember("draw_pile")
        && json.HasMember("discard_pile"))
    {
        std::vector<player*> deserialized_players;
        for (auto &serialized_player : json["players"].GetArray()) {
            deserialized_players.push_back(player::from_json(serialized_player.GetObject()));
        }
        return new game_state(reactive_object::extract_base_params(json),
                              draw_pile::from_json(json["draw_pile"].GetObject()),
                              discard_pile::from_json(json["discard_pile"].GetObject()),
                              deserialized_players,
                              reactive_value<bool>::from_json(json["is_started"].GetObject()),
                              reactive_value<bool>::from_json(json["is_finished"].GetObject()),
                              reactive_value<int>::from_json(json["current_player_idx"].GetObject()),
                              reactive_value<int>::from_json(json["play_direction"].GetObject()),
                              reactive_value<int>::from_json(json["round_number"].GetObject()),
                              reactive_value<int>::from_json(json["starting_player_idx"].GetObject()));


    } else {
        throw LamaException("Failed to deserialize game_state. Required entries were missing.");
    }
}

