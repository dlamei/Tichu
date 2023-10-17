//
// Created by Manuel on 25.01.2021.
//

#include "card.h"

#include "../../exceptions/LamaException.h"


card::card(std::string id) : unique_serializable(id) { }

card::card(std::string id, serializable_value<int> *rank, serializable_value<int> *suit,
           serializable_value<int> *val):
        unique_serializable(id), 
        _rank(rank),
        _suit(suit),
        _value(val)
{ }

card::card(int rank, int suit, int val) :
        unique_serializable(),
        _rank(new serializable_value<int>(rank)),
        _suit(new serializable_value<int>(suit)),
        _value(new serializable_value<int>(val))

{ }

card::~card() { }

int card::get_rank() const noexcept {
    return _rank->get_value();
}

int card::get_suit() const noexcept {
    return _suit->get_value();
}

int card::get_value() const noexcept {
    return _value->get_value();
}


bool card::can_be_played_on(const card *const other) const noexcept {
    return true;
    // return true if this card has a one higher or of equal value OR if 'other' is Lama and this is 1
    int value_delta = this->get_value() - other->get_value();
    return value_delta == 0 || value_delta == 1 || (other->get_value() == 7 && this->get_value() == 1);
}


card *card::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id")
        && json.HasMember("rank")
        && json.HasMember("suit")
        && json.HasMember("value"))
    {
        return new card(
                json["id"].GetString(),
                serializable_value<int>::from_json(json["rank"].GetObject()),
                serializable_value<int>::from_json(json["suit"].GetObject()),
                serializable_value<int>::from_json(json["value"].GetObject()));
    } else {
        throw LamaException("Could not parse json of card. Was missing 'id', 'rank', 'suit' or 'val'.");
    }
}


void card::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    unique_serializable::write_into_json(json, allocator);

    rapidjson::Value rank_val(rapidjson::kObjectType);
    _rank->write_into_json(rank_val, allocator);
    json.AddMember("rank", rank_val, allocator);

    rapidjson::Value suit_val(rapidjson::kObjectType);
    _suit->write_into_json(suit_val, allocator);
    json.AddMember("suit", suit_val, allocator);

    rapidjson::Value value_val(rapidjson::kObjectType);
    _value->write_into_json(value_val, allocator);
    json.AddMember("value", value_val, allocator);
}






