#include "won_cards_pile.h"

#include <utility>

#include "../../exceptions/TichuException.h"

won_cards_pile::won_cards_pile(std::vector<Card> cards) : _cards(std::move(cards)) {}

int won_cards_pile::get_score() const {
    int res = 0;
    for (Card card: _cards) {
        res += card.get_value();
    }
    return res;
}

#ifdef TICHU_SERVER

void won_cards_pile::wrap_up_round() {
    _cards.clear();
}

void won_cards_pile::add_card(const Card &new_card) {
    _cards.push_back(new_card);
}

void won_cards_pile::add_cards(const card_combination &combi){
    std::vector<Card> cards = combi.get_cards();
    for(Card card : cards) {
        add_card(card);
    }
}

void won_cards_pile::add_cards(const std::vector<card_combination> &combis) {
    for(card_combination combi : combis) {
        add_cards(combi);
    }
}



#endif


void won_cards_pile::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &alloc) const {
    vec_into_json("cards", _cards, json, alloc);
}

std::optional<won_cards_pile> won_cards_pile::from_json(const rapidjson::Value &json) {
    auto cards = vec_from_json<Card>("cards", json);

    if (!(cards)) {
        throw TichuException("Could not parse won_cards_pile from json. 'cards' were missing.");
    }

    return won_cards_pile(cards.value());
}



