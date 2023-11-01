//
// Created by Manuel on 25.01.2021.
//

#include "card.h"

#include <utility>

#include "../../exceptions/TichuException.h"


//card::card(std::string id) : unique_serializable(id) { }

card::card(UUID id, int rank, int suit, int val):
        _id(std::move(id)),
        _rank(rank),
        _suit(suit),
        _value(val) {}

card::card(int rank, int suit, int val):
        _id(UUID::create()),
        _rank(rank),
        _suit(suit),
        _value(val) {}


bool card::can_be_played_on(const card &other) const noexcept {
    return true;
    // return true if this card has a one higher or of equal value OR if 'other' is Tichu and this is 1
    int value_delta = this->get_value() - other.get_value();
    return value_delta == 0 || value_delta == 1 || (other.get_value() == 7 && this->get_value() == 1);
}


card card::from_json(const rapidjson::Value &json) {
        auto rank = primitive_from_json<int>("rank", json);
        auto suit = primitive_from_json<int>("suit", json);
        auto value = primitive_from_json<int>("value", json);
        auto id = UUID::from_json(json);

        if (rank && suit && value && id) {

            return card {
                    id.value(),
                    rank.value(),
                    suit.value(),
                    value.value()
            };
        }
        else {
            throw TichuException("Could not parse json of card. Was missing 'id', 'rank', 'suit' or 'val'.");
        }
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& alloc) const {
    _id.write_into_json(json, alloc);
    json.AddMember("rank", int_to_json(_rank, alloc), alloc);
    json.AddMember("suit", int_to_json(_suit, alloc), alloc);
    json.AddMember("value", int_to_json(_value, alloc), alloc);
}






