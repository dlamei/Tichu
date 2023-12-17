#include "won_cards_pile.h"

#include <utility>

WonCardsPile::WonCardsPile(std::vector<Card> cards) : _cards(std::move(cards)) {}

int WonCardsPile::get_score() const {
    int res = 0;
    for (Card card: _cards) {
        res += card.get_value();
    }
    return res;
}

#ifdef TICHU_SERVER

void WonCardsPile::wrap_up_round() {
    _cards.clear();
}

void WonCardsPile::add_card(const Card &new_card) {
    _cards.push_back(new_card);
}

void WonCardsPile::add_cards(const CardCombination &combi){
    std::vector<Card> cards = combi.get_cards();
    for(Card card : cards) {
        add_card(card);
    }
}

void WonCardsPile::add_cards(const std::vector<CardCombination> &combis) {
    for(const CardCombination& combi : combis) {
        add_cards(combi);
    }
}



#endif