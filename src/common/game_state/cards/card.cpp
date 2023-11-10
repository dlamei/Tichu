
#include "card.h"

#include <utility>

#include "../../exceptions/TichuException.h"


card::card(int rank, int suit, int val):
        _rank(rank),
        _suit(suit),
        _value(val) {}


bool card::can_be_played_on(const card &other) const noexcept {
    return true;
    //TODO:
    // return true if this card has a one higher or of equal value OR if 'other' is Tichu and this is 1
    int value_delta = this->get_value() - other.get_value();
    return value_delta == 0 || value_delta == 1 || (other.get_value() == 7 && this->get_value() == 1);
}


card card::from_json(const rapidjson::Value &json) {
    auto rank = int_from_json("rank", json);
    auto suit = int_from_json("suit", json);
    auto value = int_from_json("value", json);

    if (!(rank && suit && value)) {
        throw TichuException("Could not parse json of card. Was missing 'rank', 'suit' or 'val'.");
    }

    return card {
            rank.value(),
            suit.value(),
            value.value()
    };
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& alloc) const {
    int_into_json("rank", _rank, json, alloc);
    int_into_json("suit", _suit, json, alloc);
    int_into_json("value", _value, json, alloc);
}






