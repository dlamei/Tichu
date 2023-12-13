#include "GameState.h"

#include <iostream>
#include <utility>
#include <memory>
#include <shared_mutex>
#include <random>

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

int GameState::get_player_index(UUID player_id) const {
    for (int i = 0; i < _players.size(); ++i) {
        if(_players.at(i)->get_id() == player_id) { return i;}
    }
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

    if (_game_phase == GamePhase::PREGAME) {
        // make teams
        make_teams();
        
        this->setup_round(err);
        this->_game_phase = GamePhase::INROUND; //SHOULD BE PREROUND
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
    _game_phase = GamePhase::POSTGAME;
}

void GameState::make_teams(){
    int number_of_A = 0;
    int number_of_B = 0;
    for(auto player : _players){
        if((*player).get_team() == Team::A) { ++number_of_A; }
        if((*player).get_team() == Team::A) { ++number_of_B; }
    }

    int swap_location = 0;
    if(number_of_A == 2 && number_of_B == 2){
        for(auto player : _players){
            if((*player).get_team() == Team::A){
                std::swap(player, _players.at(swap_location));
                swap_location = 2;
            }
        }
    }
    else {
        std::shuffle(_players.begin(), _players.end(), std::mt19937(std::random_device()()));
    }

    // change team variable of all players 
    (*_players.at(0)).set_team(Team::A);
    (*_players.at(1)).set_team(Team::B);
    (*_players.at(2)).set_team(Team::A);
    (*_players.at(3)).set_team(Team::B);
}

bool GameState::call_grand_tichu(const Player &player, Tichu tichu, std::string &err) {
    if(_game_phase != GamePhase::PREROUND) {
        err = "You can't call a Grand Tichu Anymore";
        return false;
    }

    static int call_count = 0;
    ++call_count;

    int player_idx = get_player_index(player);
    _players.at(player_idx)->set_tichu(tichu);

    if(call_count < 4){
        return true;
    } else {
        call_count = 0;
        _game_phase = GamePhase::SWAPPING;
        return true;
    }
}

bool GameState::call_small_tichu(const Player &player, Tichu tichu, std::string &err) {
    if(player.get_hand().get_nof_cards() != 14) {
        err = "You can't call a Tichu Anymore";
        return false;
    }
    int player_idx = get_player_index(player);
    _players.at(player_idx)->set_tichu(Tichu::TICHU);
    return true;
}

bool GameState::dragon_selection(const Player &player, UUID selected_player, std::string &err) {
    _players.at(get_player_index(selected_player))->add_cards_to_won_pile(_active_pile.get_pile(), err);
    _active_pile.clear_cards();
    _game_phase = GamePhase::INROUND;
    return true;
}

bool GameState::swap_cards(const Player &player, const std::vector<Card> &cards, 
                           std::vector<std::vector<Card>> swapped_cards, std::string &err) {
    if( _game_phase != GamePhase::SWAPPING ) {
        err = "You can't swap cards anymore";
        return false;
    }
    if( cards.size() != 3 ) {
        err = "Card swap Vector has the wrong size";
        return false;
    }
    if( (cards.at(0) == cards.at(1)) || (cards.at(0) == cards.at(2)) || (cards.at(1) == cards.at(2)) ) {
        err = "You can't swap the same card to two different players";
        return false;
    }
    
    
    static std::vector<std::vector<Card>> swap_tracker = {  {{},{},{},{}},
                                                            {{},{},{},{}},
                                                            {{},{},{},{}},
                                                            {{},{},{},{}}  };
    static int call_count = 0;
    ++call_count;

    // add cards into swap_tracker
    int player_idx = get_player_index(player);
    for(int i = 0; i < 3; ++i) {
        swap_tracker.at(player_idx).at(i + (i >= player_idx)) = cards.at(i);
    }


    if(call_count < 4){
        return true;
    } else {
        call_count = 0;
        _game_phase = GamePhase::INROUND;

        // moving all the cards
        for(int i = 0; i < 4; ++i) {
            for(int j = 0; j < 4; ++j) {
                if(i == j) { continue; }
                _players.at(i)->remove_cards_from_hand(CardCombination{swap_tracker.at(i).at(j)}, err);
                _players.at(j)->add_card_to_hand(swap_tracker.at(i).at(j), err);
            }
        }

        // copying cards into return matrix
        swapped_cards = swap_tracker;

        // resetting the swap_tracker
        swap_tracker = {  {{},{},{},{}},
                          {{},{},{},{}},
                          {{},{},{},{}},
                          {{},{},{},{}}  };

        return true;
    }
}

bool GameState::check_wish(const CardCombination &combi, const Player &player, const std::optional<Card> &wish, std::string &err)
{   
    // the played combi itsself is a wish
    if(wish) {
        if(wish.value().get_rank() == SPECIAL) {
            err = "You can't wish for a special card";
            return false;
        }
        _wish = wish;
        return true;
    }
    // there currently is no wish
    if( !_wish ) {
        return true;
    } 
    // there is a current active wish

    else {
        if(player.get_hand().count_occurances(_wish.value())) {
            if(combi.count_occurances(_wish.value())) {
                // the player has the wished for card and is playing it
                _wish = {};
                return true;
            } else {
                err = "You must play the wished for card: " + combi.get_cards().at(0).to_string();
                return false;
            }
        } 
        // The player doesn't have the wished for car in his hand
        else {
            return true;
        }
    }
}

// 
//   [ROUND FUNCTIONS] 
// 
void GameState::setup_round(std::string &err) {
    _game_phase = GamePhase::PREROUND;

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

    // figure out who goes first
        for(int i = 0; i < 4; ++i) {
            auto cards = _players.at(i)->get_hand().get_cards();
            if(count(cards.begin(), cards.end(), ONE)) {
                _next_player_idx = i;
                break;
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
    if(_players.at(1)->get_is_finished() && _players.at(3)->get_is_finished()) {
        return true;
    }
    // round not over
    return false;
}

void GameState::wrap_up_round(Player &current_player, std::string& err) {
    _is_round_finished = true;
    _wish = {};

    int first_player_idx = get_player_index(_round_finish_order.at(0));
    int last_player_idx = _next_player_idx;
    // team A doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished() 
       && !(_players.at(1)->get_is_finished()) && !(_players.at(3)->get_is_finished())) {
        _score_team_A += 200;
    }
    // team B doppelsieg
    else if(_players.at(1)->get_is_finished() && _players.at(3)->get_is_finished()
            && !(_players.at(0)->get_is_finished()) && !(_players.at(2)->get_is_finished())) {
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
    if(_players.at(0)->get_tichu() == Tichu::GRAND_TICHU) { 
        if(first_player_idx == 0) {
            _score_team_A += 200;
        } else { _score_team_A -= 200; }
    } else if (_players.at(0)->get_tichu() == Tichu::TICHU) {
        if(first_player_idx == 0) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }
    if(_players.at(1)->get_tichu() == Tichu::GRAND_TICHU) { 
        if(first_player_idx == 0) {
            _score_team_B += 200;
        } else { _score_team_B -= 200; }
    } else if (_players.at(1)->get_tichu() == Tichu::TICHU) {
        if(first_player_idx == 0) {
            _score_team_B += 100;
        } else { _score_team_B -= 100; }
    }
    if(_players.at(2)->get_tichu() == Tichu::GRAND_TICHU) { 
        if(first_player_idx == 0) {
            _score_team_A += 200;
        } else { _score_team_A -= 200; }
    } else if (_players.at(2)->get_tichu() == Tichu::TICHU) {
        if(first_player_idx == 0) {
            _score_team_A += 100;
        } else { _score_team_A -= 100; }
    }
    if(_players.at(3)->get_tichu() == Tichu::GRAND_TICHU) { 
        if(first_player_idx == 0) {
            _score_team_B += 200;
        } else { _score_team_B -= 200; }
    } else if (_players.at(3)->get_tichu() == Tichu::TICHU) {
        if(first_player_idx == 0) {
            _score_team_B += 100;
        } else { _score_team_B -= 100; }
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
    // everyone skipped or is finished or is the last to have played a card
    bool everyone_skipped = true;
    for(int i = 0; i < 4; ++i) {
        if(i == _last_player_idx || (*_players.at(i)).get_has_skipped() || (*_players.at(i)).get_is_finished()){
            continue;
        } else {
            everyone_skipped = false;
            break;
        }
    }
    if(everyone_skipped) { return true; }

    // team A doppelsieg
    if(_players.at(0)->get_is_finished() && _players.at(2)->get_is_finished()) {
        return true;
    }
    // team B doppelsieg
    if(_players.at(1)->get_is_finished() && _players.at(3)->get_is_finished()) {
        return true;
    }
    return false;
}

void GameState::wrap_up_trick(Player &Player, std::string &err) {
    _is_trick_finished = true;
    // move cards to WonCardsPile to right Player

    // Handle Dragon Stich
    std::optional<CardCombination> top_combi = _active_pile.get_top_combi();
    if(top_combi && !(top_combi.value().get_cards().empty()) && top_combi.value().get_cards().at(0) == DRAGON) {
        if(_players.at((_last_player_idx + 1) % 4)->get_is_finished()) {
            _players.at((_last_player_idx + 3) % 4)->add_cards_to_won_pile(_active_pile.get_pile(), err);
            _active_pile.clear_cards();
        }
        else if(_players.at((_last_player_idx + 3) % 4)->get_is_finished()) {
            _players.at((_last_player_idx + 1) % 4)->add_cards_to_won_pile(_active_pile.get_pile(), err);
            _active_pile.clear_cards();
        } else {
            _game_phase = GamePhase::SELECTION;
            return;
        }
    }

    _players.at(_last_player_idx)->add_cards_to_won_pile(_active_pile.get_pile(), err);
    _active_pile.clear_cards();
}


// 
//   [PLAYER FUNCTIONS] 
// 
bool GameState::add_player(const player_ptr player_ptr, std::string& err) {
    if (_game_phase == GamePhase::PREROUND || _game_phase == GamePhase::INROUND) {
        err = "Could not join game, because the requested game is already started.";
        return false;
    }
    if (_game_phase == GamePhase::POSTGAME) {
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

//   [Game Logic]
bool GameState::play_combi(Player &Player, CardCombination& combi, std::string &err, std::optional<Card> wish) {
    int player_idx = get_player_index(Player);
    if(player_idx < 0 || player_idx > 3){
        err = "couldn't find Player index";
        return false;
    }
    if (!is_player_in_game(Player)) {
        err = "Server refused to perform draw_card. Player is not part of the game.";
        return false;
    }
    if (!is_allowed_to_play_now(Player) && combi.get_combination_type() != BOMB) {
        err = "It's not this players turn yet.";
        return false;
    }

    switch(_game_phase){
        case GamePhase::PREROUND:
            err = "Could not play combi, people are still choosing to call a Grand Tichu.";
            return false;
        case GamePhase::SWAPPING: 
            err = "Could not play combi, because the game is still in the swapping phase.";
            return false;
        case GamePhase::POSTGAME: 
            err = "Could not play combi, because the requested game is already finished.";
            return false;
        case GamePhase::PREGAME: 
            err = "Could not play combi, because the game hasn't started yet.";
            return false;
        case GamePhase::SELECTION: 
            err = "Could not play combi, because someone is currently choosing who to give the Tichu stich to.";
            return false;
        default:
            break;
    }

    _is_round_finished = false;
    _is_trick_finished = false;

    auto last_combi = _active_pile.get_top_combi();

    // check if it's legal to play this combination
    if(combi.can_be_played_on(last_combi, err) && check_wish(combi, Player, wish, err)){

        // move cards
        if(combi.get_combination_type() != PASS){
            _active_pile.push_active_pile(combi);
            Player.remove_cards_from_hand(combi, err);
            for(auto player : _players) {
                (*player).set_skipped(false);
            }
        } else {
            Player.set_skipped(true);
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