#include "hand.h"

#include <utility>
#include <algorithm>

#include "../../exceptions/TichuException.h"

hand::hand(std::vector<Card> cards) : _cards(std::move(cards)) { }

int hand::get_score() const {
    int res = 0;
    for(Card card : _cards) {
        res += card.get_value();
    }
    return res;
}

std::optional<Card> hand::try_get_card(const Card &card) const {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card](const class Card &x) { return x == card;});
    if (it < _cards.end()) {
        return *it;
    }
    return {};
}


std::optional<Card> hand::remove_card(int idx) {
    return remove_card(_cards.begin() + idx);
}

std::optional<Card> hand::remove_card(const Card& card) {
    auto pos = std::find(_cards.begin(), _cards.end(), card);
    return remove_card(pos);
}

std::optional<Card> hand::remove_card(std::vector<Card>::iterator pos) {
    if (pos >= _cards.begin() && pos < _cards.end()) {
        Card res = *pos;
        _cards.erase(pos);
        return res;
    }
    return {};
}


#ifdef TICHU_SERVER

int hand::wrap_up_round() {
    int score = get_score();
    _cards.clear();
    return score;
}

bool hand::add_card(const Card &new_card, std::string &err) {
    _cards.push_back(new_card);
    std::sort(_cards.begin(),_cards.end());
    return true;
}

void hand::add_cards(const std::vector<Card> &cards, std::string &err) {
    for(Card card : cards) {
        add_card(card, err);
    }
}

std::optional<Card> hand::remove_card(const Card &card, std::string &err) {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card](const class Card &x) { return x == card;});
    if (it < _cards.end()) {
        return remove_card(it);
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return {};
    }
}

bool hand::remove_cards(const std::vector<Card> &cards, std::string& err) {
        for(Card card : cards) {
            if(!(remove_card(card, err))){
                return false;
            }
        }
        return true;
}

#endif


void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& alloc) const {
    vec_into_json("cards", _cards, json, alloc);
}

std::optional<hand> hand::from_json(const rapidjson::Value &json) {
    auto cards = vec_from_json<Card>("cards", json);

    if (!(cards)) {
        throw TichuException("Could not parse hand from json. 'cards' were missing.");
    }

    return hand(cards.value());
}



