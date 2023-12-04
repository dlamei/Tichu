#include "GameState.h"

#include <iostream>
#include <utility>
#include <memory>
#include <shared_mutex>

GameState::GameState() :
        _id(UUID::create()) {}

GameState::GameState(UUID id) :
        _id(std::move(id)) {}

// accessors
std::optional<Player> GameState::get_current_player() const {
    if (_players.empty()) {
        return {};
    }
    return *(_players[_next_player_idx]);
}

int GameState::get_player_index(const Player &player) const {
    for (int i = 0; i < _players.size(); ++i) {
        if (*(_players.at(i)) == player) { return i; }
    }
    return -1;
}

bool GameState::is_player_in_game(const Player &player) const {
    for (int i = 0; i < _players.size(); ++i) {
        if (*(_players.at(i)) == player) { return true; }
    }
    return false;
}

bool GameState::is_allowed_to_play_now(const Player &player) const {
    auto current = get_current_player();
    if (!current) { // no current Player
        return false;
    }
    return player == current.value();
}


#ifdef TICHU_SERVER
// 
//   [ FUNCTIONS] 
// 
bool GameState::start_game(std::string &err) {
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

bool GameState::check_is_game_over(std::string& err) {
    if(_score_team_A >= 1000){ return true; }
    if(_score_team_B >= 1000){ return true; }
    return false;
}

void GameState::wrap_up_game(std::string& err) {
    _is_game_finished = true;
}

// 
//   [ROUND FUNCTIONS] 
// 
void GameState::setup_round(std::string &err) {


    // setup DrawPile
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

bool GameState::check_is_round_finished(Player &Player, std::string& err) {
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

void GameState::wrap_up_round(Player &current_player, std::string& err) {
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
        for(auto Player : _players) {
            won_cards_scores.push_back(Player->get_won_score());
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
    for(auto Player : _players) {
        Player->wrap_up_round(err);
    }    
    _round_finish_order.clear();
}

// 
//   [TRICK FUNCTIONS] 
// 
void GameState::setup_trick(Player &Player, std::string &err) {

}

bool GameState::check_is_trick_finished(Player &Player, std::string& err) {
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

void GameState::wrap_up_trick(Player &Player, std::string &err) {
    _is_trick_finished = true;
    // move cards to WonCardsPile to right Player
    _players.at(_last_player_idx)->add_cards_to_won_pile(_active_pile.get_pile(), err);
    _active_pile.clear_cards();
}


// 
//   [PLAYER FUNCTIONS] 
// 
bool GameState::add_player(const player_ptr player_ptr, std::string& err) {
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
        err = "Could not join game, because this Player is already subscribed to this game.";
        return false;   
        }
    }

    _players.push_back(player_ptr);
    return true;
}

void GameState::update_current_player(Player &Player, bool is_dog, std::string& err) {
    if(is_dog) {
        _next_player_idx = (_next_player_idx + 1) % 4;
    }
    do {
        _next_player_idx = (_next_player_idx + 1) % 4;
    } while (_players.at(_next_player_idx)->get_is_finished());
}

bool GameState::remove_player(player_ptr player_ptr, std::string &err) {
    int idx = get_player_index(*player_ptr);
    if (idx != -1) {
        if (idx < _next_player_idx) {
            // reduce next_player_idx if the Player who left had a lower index
            _next_player_idx -= 1;
        }
        _players.erase(_players.begin() + idx);
        return true;
    } else {
        err = "Could not leave game, as the requested Player was not found in that game.";
        return false;
    }
}

// 
//   [TURN FUNCTIONS] 
// 
void GameState::setup_player(Player &Player, std::string &err) {

}

bool GameState::check_is_player_finished(Player &Player, std::string &err) {
    return Player.get_nof_cards() == 0;
}

void GameState::wrap_up_player(Player &Player, std::string &err) {

    _round_finish_order.push_back(Player);
    Player.finish();

}

//   [GamePanel Logic]
bool GameState::play_combi(Player &Player, CardCombination& combi, std::string &err) {
    _is_round_finished = false;
    _is_trick_finished = false;
    int player_idx = get_player_index(Player);
    if(player_idx < 0 || player_idx > 3){
        err = "couldn't find Player index";
        return false;
    }
    if (!is_player_in_game(Player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(Player)) {
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
            Player.remove_cards_from_hand(combi, err);
        }

        //update Player
        bool is_dog = combi.get_combination_type() == SWITCH;
        update_current_player(Player,is_dog, err);
        

        // checks if Player, trick, round or game is finished
        if( check_is_player_finished(Player, err) ) {
            wrap_up_player(Player, err);
        } 
        if( check_is_trick_finished(Player, err) ) {
            wrap_up_trick(Player, err);
            if( check_is_round_finished(Player, err) ) {
                wrap_up_round(Player, err);
                if( check_is_game_over(err) ) { 
                    wrap_up_game(err);  
                }
            setup_round(err); 
            }
        setup_trick(Player, err);
        }
        if(combi.get_combination_type() != PASS){
        _last_player_idx = get_player_index(Player);
        }
        return true;

    } else {

        return false;

    }

}

#endif