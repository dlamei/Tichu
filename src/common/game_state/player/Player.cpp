#include "Player.h"

#include <utility>

Player::Player(std::string name) :
        _id(UUID::create()),
        _player_name(std::move(name)) {}

#ifdef TICHU_SERVER

Player::Player(UUID id, std::string name, Team team) :
        _id(id),
        _player_name(name),
        _team(team)
{ }

bool Player::add_card_to_hand(const Card &card, std::string &err) {
    return _hand.add_card(card, err);
}

void Player::remove_cards_from_hand(const CardCombination &combi, std::string& err) {
    _hand.remove_cards(combi.get_cards(), err);
}

bool Player::add_cards_to_won_pile(const std::vector<CardCombination> &combis, std::string& err) {
    _won_cards.add_cards(combis);
    return true;
}

void Player::wrap_up_round(std::string &err) {
    _hand.wrap_up_round();
    _won_cards.wrap_up_round();
    _is_finished = false;
    _tichu = false;
    _grand_tichu = false;
}

#endif