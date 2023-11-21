
#include "card.h"

#include <utility>

#include "../../exceptions/TichuException.h"


Card::Card(int rank, int suit, int val) : _rank(rank), _suit(suit), _value(val) {}

Card::Card(int rank, int suit) : _rank(rank), _suit(suit) {
    int value = 0;
    if(rank == 5) { value = 5; }
    else if(rank == 10 || rank == 13) { value = 10;}
    else if(rank == 1 && suit == 1) { value = -25; }
    else if(rank == 1 && suit == 2) { value = 25; }
    _value = value;
}


Card Card::from_json(const rapidjson::Value &json) {
    auto rank = int_from_json("rank", json);
    auto suit = int_from_json("suit", json);
    auto value = int_from_json("value", json);

    if (!(rank && suit && value)) {
        throw TichuException("Could not parse json of card. Was missing 'rank', 'suit' or 'val'.");
    }

    return Card {
            rank.value(),
            suit.value(),
            value.value()
    };
}


void Card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& alloc) const {
    int_into_json("rank", _rank, json, alloc);
    int_into_json("suit", _suit, json, alloc);
    int_into_json("value", _value, json, alloc);
}






