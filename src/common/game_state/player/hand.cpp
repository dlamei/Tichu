#include "hand.h"

#include <utility>

#include "../../exceptions/TichuException.h"
#include "../../serialization/vector_utils.h"

hand::hand(): _id(UUID::create()) { }

hand::hand(UUID id) : _id(std::move(id)) { }

// deserialization constructor
hand::hand(UUID id, std::vector<card> cards) : _id(std::move(id)), _cards(std::move(cards)) { }

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

std::optional<card> hand::try_get_card(const UUID &card_id) const {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card& x) { return x.get_id() == card_id;});
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

std::optional<card> hand::remove_card(const UUID &card_id, std::string &err) {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card &x) { return x.get_id() == card_id;});
    if (it < _cards.end()) {
        return remove_card(it);
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return {};
    }
}
#endif


void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    _id.write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}

std::optional<hand> hand::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card> deserialized_cards {};
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        auto id = UUID::from_json(json);
        return hand(id.value(), deserialized_cards);
    } else {
        throw TichuException("Could not parse hand from json. 'cards' were missing.");
    }
}



