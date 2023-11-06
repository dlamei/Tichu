#include "hand.h"

#include <utility>

#include "../../exceptions/TichuException.h"

hand::hand(std::vector<card> cards) : _cards(std::move(cards)) { }

int hand::get_score() const {
    int res = 0;
    bool already_counted[7] = { false, false, false, false, false, false, false }; // for the 7 card types
    for (const auto & card : _cards) {
        int card_value = card.get_value();
        if(!already_counted[card_value - 1]) {
            already_counted[card_value - 1] = true;
            if(card_value == 7) {
                res += 10;
            } else {
                res += card_value;
            }
        }
    }
    return res;
}

std::optional<card> hand::try_get_card(const card &card) const {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card](const class card &x) { return x == card;});
    if (it < _cards.end()) {
        return *it;
    }
    return {};
}


std::optional<card> hand::remove_card(int idx) {
    return remove_card(_cards.begin() + idx);
}

std::optional<card> hand::remove_card(const card& card) {
    auto pos = std::find(_cards.begin(), _cards.end(), card);
    return remove_card(pos);
}

std::optional<card> hand::remove_card(std::vector<card>::iterator pos) {
    if (pos >= _cards.begin() && pos < _cards.end()) {
        card res = *pos;
        _cards.erase(pos);
        return res;
    }
    return {};
}

std::vector<card>::iterator hand::get_card_iterator() {
    return _cards.begin();
}


#ifdef TICHU_SERVER
void hand::setup_round(std::string &err) {
    // remove all cards (if any) and clear it
    _cards.clear();
}

bool hand::add_card(const card &new_card, std::string &err) {
    _cards.push_back(new_card);
    return true;
}

std::optional<card> hand::remove_card(const card &card, std::string &err) {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card](const class card &x) { return x == card;});
    if (it < _cards.end()) {
        return remove_card(it);
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return {};
    }
}
#endif


void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& alloc) const {
    vec_into_json("cards", _cards, json, alloc);
}

std::optional<hand> hand::from_json(const rapidjson::Value &json) {
    auto cards = vec_from_json<card>("cards", json);

    if (!(cards)) {
        throw TichuException("Could not parse hand from json. 'cards' were missing.");
    }

    return hand(cards.value());
}



