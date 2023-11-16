//
// Created by Manuel on 27.01.2021.
//
#include "game_state.h"

#include <iostream>
#include <utility>
#include <memory>
#include <shared_mutex>

#include "../exceptions/TichuException.h"

enum COMBI{
    NONE, SINGLE, DOUBLE, TRIPLE, BOMB, FULLHOUSE, STRASS, TREPPE, PASS, SWITCH
};

game_state::game_state() :
        _id(UUID::create()),

        _players(std::vector<player_ptr>()),
        _round_finish_order(std::vector<player>()),
      
        _draw_pile(draw_pile()),
        _active_pile(active_pile()),

        _score_team_A(0),
        _score_team_B(0),

        _next_player_idx(0),
        _starting_player_idx(0),

        _is_started(false),
        _is_game_finished(false),
        _is_round_finished(false),
        _is_trick_finished(false),

        _last_player_idx(0)
{ }

game_state::game_state(UUID id) : 
                _id(std::move(id)),

                _players(std::vector<player_ptr>()),
                _round_finish_order(std::vector<player>()),

                _draw_pile(draw_pile()),
                _active_pile(active_pile()),

                _score_team_A(0),
                _score_team_B(0),

                _next_player_idx(0),
                _starting_player_idx(0),

                _is_started(false),
                _is_game_finished(false),
                _is_round_finished(false),
                _is_trick_finished(false),

                _last_player_idx(0)
                                  
{ }

game_state::game_state(UUID id, 
                       std::vector<player_ptr>& players, 
                       std::vector<player>& round_finish_order, 
                       draw_pile draw_pile, 
                       active_pile active_pile,
                       int score_team_A,
                       int score_team_B,
                       int next_player_idx,
                       int starting_player_idx,
                       bool is_started,
                       bool is_game_finished, 
                       bool is_round_finished, 
                       bool is_trick_finished, 
                       int last_player_idx
                       ) : 
                _id(std::move(id)),

                _players(players),
                _round_finish_order(round_finish_order),
            
                _draw_pile(draw_pile),
                _active_pile(active_pile),
        
                _score_team_A(score_team_A),
                _score_team_B(score_team_B),
        
                _next_player_idx(next_player_idx),
                _starting_player_idx(starting_player_idx),
        
                _is_started(is_started),
                _is_game_finished(is_game_finished),
                _is_round_finished(is_round_finished),
                _is_trick_finished(is_trick_finished),
        
                _last_player_idx(last_player_idx)
{ }         

// accessors
std::optional<player> game_state::get_current_player() const {
    if(_players.empty()) {
        return {};
    }
    return *(_players[_next_player_idx]);
}

int game_state::get_player_index(const player &player) const {
    for(int i = 0; i < _players.size(); ++i) {
        if(*(_players.at(i)) == player) { return i; }
    }
    return -1;
}

bool game_state::is_player_in_game(const player &player) const {
    for(int i = 0; i < _players.size(); ++i) {
        if(*(_players.at(i)) == player) { return true; }
    }
    return false;
}

bool game_state::is_allowed_to_play_now(const player &player) const {
    auto current = get_current_player();
    if (!current) { // no current player
        return false;
    }
    return player == current.value();
}


#ifdef TICHU_SERVER
// 
//   [ FUNCTIONS] 
// 
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

bool game_state::check_is_game_over(std::string& err) { 
    if(_score_team_A >= 1000){ return true; }
    if(_score_team_B >= 1000){ return true; }
    return false;
}

void game_state::wrap_up_game(std::string& err) {
    _is_game_finished = true;
}

// 
//   [ROUND FUNCTIONS] 
// 
void game_state::setup_round(std::string &err) {


    // setup draw_pile
    _draw_pile.setup_game(err);

    // setup players
    for (int i = 0; i < _players.size(); i++) {
        _players[i]->setup_round(err);
        // draw 14 cards
        //card* drawn_card = nullptr;
        for (int j = 0; j < 14; j++) {
            auto drawn_card = _draw_pile.draw(*(_players[i]), err);
            if (!drawn_card) {
                std::cerr << err << std::endl;
            }
        }
    }

    
}

bool game_state::check_is_round_finished(player &player, std::string& err) {
    // 3 players finished
    if(_round_finish_order.size() >= 3) {
        return true;
    }
    // team A doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        return true;
    }
    // team B doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        return true;
    }
    // round not over
    return false;
}

void game_state::wrap_up_round(player &current_player, std::string& err) {
    _is_round_finished = true;

    int first_player_idx = get_player_index(_round_finish_order.at(0));
    int last_player_idx = _next_player_idx;
    // team A doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        _score_team_A += 200;
    }
    // team B doppelsieg
    else if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        _score_team_B += 200;
    } else {
        
        // Update Score
        std::vector<int> won_cards_scores;
        for(auto player : _players) {
            won_cards_scores.push_back(player->get_won_score());
        }
        won_cards_scores.at(first_player_idx) += won_cards_scores.at(last_player_idx);
        won_cards_scores.at(last_player_idx) = 0;
        won_cards_scores.at((last_player_idx + 1) % 4) += _players.at(last_player_idx)->get_hand_score();

        _score_team_A += (won_cards_scores.at(0) + won_cards_scores.at(2));
        _score_team_B += (won_cards_scores.at(1) + won_cards_scores.at(3));
    }
    //check for failed or successful tichu
    if(_players.at(0)->get_tichu()) { 
        if(first_player_idx == 0) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }
    if(_players.at(1)->get_tichu()) { 
        if(first_player_idx == 1) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }
    if(_players.at(2)->get_tichu()) { 
        if(first_player_idx == 2) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }
    if(_players.at(3)->get_tichu()) { 
        if(first_player_idx == 3) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }

    // clear and wrapup stuff
    _starting_player_idx = first_player_idx;
    _next_player_idx = first_player_idx;
    _active_pile.clear_cards();
    _last_player_idx = 4;
    for(auto player : _players) {
        player->wrap_up_round(err);
    }    
    _round_finish_order.clear();
}

// 
//   [TRICK FUNCTIONS] 
// 
void game_state::setup_trick(player &player, std::string &err) { 

}

bool game_state::check_is_trick_finished(player &player, std::string& err) {
    // everyone skipped
    if(_last_player_idx == _next_player_idx) {
        return true;
    }
    // team A doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        return true;
    }
    // team B doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        return true;
    }
    return false;
}

void game_state::wrap_up_trick(player &player, std::string &err) {
    _is_trick_finished = true;
    // move cards to won_cards_pile to right player
    _players.at(_last_player_idx)->add_cards_to_won_pile(_active_pile.get_pile(), err);
    _active_pile.clear_cards();
}


// 
//   [PLAYER FUNCTIONS] 
// 
bool game_state::add_player(const player_ptr player_ptr, std::string& err) {
    if (_is_started) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_is_game_finished) {
        err = "Could not join game, because the requested game is already finished.";
        return false;
    }
    if (_players.size() >= _max_nof_players) {
        err = "Could not join game, because the max number of players is already reached.";
        return false;
    }
    for(int i = 0; i < _players.size(); ++i) {
        if(*(_players.at(i)) == *player_ptr) { 
        err = "Could not join game, because this player is already subscribed to this game.";
        return false;   
        }
    }

    _players.push_back(player_ptr);
    return true;
}

void game_state::update_current_player(player &player, bool is_dog, std::string& err) {
    if(is_dog) {
        _next_player_idx = (_next_player_idx + 1) % 4;
    }
    do {
        _next_player_idx = (_next_player_idx + 1) % 4;
    } while (_players.at(_next_player_idx)->get_is_finished());
}

bool game_state::remove_player(player_ptr player_ptr, std::string &err) {
    int idx = get_player_index(*player_ptr);
    if (idx != -1) {
        if (idx < _next_player_idx) {
            // reduce next_player_idx if the player who left had a lower index
            _next_player_idx -= 1;
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested player was not found in that game.";
        return false;
    }
}

// 
//   [TURN FUNCTIONS] 
// 
void game_state::setup_player(player &player, std::string &err) {

}

bool game_state::check_is_player_finished(player &player, std::string &err) {
    return player.get_nof_cards() == 0;
}

void game_state::wrap_up_player(player &player, std::string &err) {

    _round_finish_order.push_back(player);
    player.finish();

}

//   [Game Logic]
bool game_state::play_combi(player &player, card_combination& combi, std::string &err) {
    _is_round_finished = false;
    _is_trick_finished = false;
    int player_idx = get_player_index(player);
    if(player_idx < 0 || player_idx > 3){
        err = "couldn't find player index";
        return false;
    }
    if (!is_player_in_game(player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(player)) {
        err = "It's not this players turn yet.";
        return false;
    }
    if (_is_game_finished) {
        err = "Could not play card, because the requested game is already finished.";
        return false;
    }

    auto last_combi = _active_pile.get_top_combi();
    // check if it's legal to play this combination
    if(combi.can_be_played_on(last_combi, err)){

        // move cards
        if(combi.get_combination_type() != PASS){
            _active_pile.push_active_pile(combi);
            player.remove_cards_from_hand(combi, err);
        }

        //update player
        bool is_dog = combi.get_combination_type() == SWITCH;
        update_current_player(player,is_dog, err);
        

        // checks if player, trick, round or game is finished
        if( check_is_player_finished(player, err) ) { 
            wrap_up_player(player, err); 
        } 
        if( check_is_trick_finished(player, err) ) { 
            wrap_up_trick(player, err); 
            if( check_is_round_finished(player, err) ) { 
                wrap_up_round(player, err); 
                if( check_is_game_over(err) ) { 
                    wrap_up_game(err);  
                }
            setup_round(err); 
            }
        setup_trick(player, err); 
        }
        if(combi.get_combination_type() != PASS){
        _last_player_idx = get_player_index(player);
        }
        return true;

    } else {

        return false;

    }

}

#endif



// Serializable interface
void game_state::write_into_json(rapidjson::Value &json,
                                 rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {

    string_into_json("id", _id.string(), json, alloc);

    std::vector<player> players;
    for(auto player : _players) { players.push_back(*player); }

    vec_into_json("players", players, json, alloc);
    vec_into_json("round_finish_order", _round_finish_order, json, alloc);

    _draw_pile.write_into_json_obj("draw_pile", json, alloc);
    _active_pile.write_into_json_obj("active_pile", json, alloc);

    int_into_json("next_player_idx", _next_player_idx, json, alloc);
    int_into_json("starting_player_idx", _starting_player_idx, json, alloc);

    int_into_json("score_team_A", _score_team_A, json, alloc);
    int_into_json("score_team_B", _score_team_B, json, alloc);

    bool_into_json("is_game_finished", _is_game_finished, json, alloc);
    bool_into_json("is_round_finished", _is_round_finished, json, alloc);
    bool_into_json("is_trick_finished", _is_trick_finished, json, alloc);
    bool_into_json("is_started", _is_started, json, alloc);

    int_into_json("last_player_idx", _last_player_idx, json, alloc);

}

game_state game_state::from_json(const rapidjson::Value &json) {
    //TODO: remove check

        auto id = string_from_json("id", json);

        auto players = vec_from_json<player>("players", json);
        std::vector<player_ptr> player_ptrs;
        if(players){
        for(auto plyr : players.value()) {
            player_ptrs.push_back(std::make_shared<player>(plyr));
        }
        }
        auto round_finish_order = vec_from_json<player>("round_finish_order", json);

        auto draw_pile = draw_pile::from_json(json["draw_pile"].GetObject());
        auto active_pile = active_pile::from_json(json["active_pile"].GetObject());

        auto next_player_idx = int_from_json("next_player_idx", json);
        auto starting_player_idx = int_from_json("starting_player_idx", json);

        auto score_team_A = int_from_json("score_team_A", json);
        auto score_team_B = int_from_json("score_team_B", json);


        auto is_started = bool_from_json("is_started", json);
        auto is_game_finished = bool_from_json("is_game_finished", json);
        auto is_round_finished = bool_from_json("is_round_finished", json);
        auto is_trick_finished = bool_from_json("is_trick_finished", json);

        auto last_player_idx = int_from_json("last_player_idx", json);

        if (id && players && round_finish_order /*&& draw_pile 
            && active_pile */ && score_team_A && score_team_B 
            && next_player_idx && starting_player_idx 
            && is_started && is_game_finished && is_round_finished 
            && is_trick_finished && last_player_idx) {
            return game_state {
                    UUID(id.value()),
                    player_ptrs,
                    round_finish_order.value(),
                    draw_pile,
                    active_pile,
                    score_team_A.value(),
                    score_team_B.value(),
                    next_player_idx.value(),
                    starting_player_idx.value(),
                    is_started.value(),
                    is_game_finished.value(),
                    is_round_finished.value(),
                    is_trick_finished.value(),
                    last_player_idx.value()
            };

        } else {
            throw TichuException("Failed to deserialize game_state. Required entries were missing.");
        }
}

