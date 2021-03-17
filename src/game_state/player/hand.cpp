//
// Created by Manuel on 27.01.2021.
//

#include "hand.h"

#include <algorithm>

#include "../../common/utils/LamaException.h"
#include "../../reactive_state/array_helpers.h"
#include "../../reactive_state/diffable_utils.h"
#include "../../reactive_state/diffs/array_diff.h"

hand::hand() : reactive_object("hand") { }

hand::hand(reactive_object::base_params params) : reactive_object(params) { }

// deserialization cosntructor
hand::hand(base_params params, std::vector<card*> cards) : reactive_object(params) {
    this->_cards = cards;
}

hand::~hand() {
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards.at((i));
        _cards.at(i) = nullptr;
    }
    _cards.clear();
}

int hand::get_nof_cards() const {
    return _cards.size();
}

int hand::get_score() const {
    int res = 0;
    for (int i = 0; i < _cards.size(); i++) {
        res += _cards[i]->get_value();
    }
    return res;
}

const std::vector<card*> hand::get_cards() const {
    return _cards;
}

bool hand::try_get_card(const std::string &card_id, card *&hand_card) const {
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        hand_card = *it;
        return true;
    }
    return false;
}


card* hand::remove_card(int idx) {
    return remove_card(_cards.begin() + idx);
}

card* hand::remove_card(card* card) {
    auto pos = std::find(_cards.begin(), _cards.end(), card);
    return remove_card(pos);
}

card* hand::remove_card(std::vector<card*>::iterator pos) {
    if (pos >= _cards.begin() && pos < _cards.end()) {
        card* res = *pos;
        _cards.erase(pos);
        return res;
    }
    return nullptr;
}

std::vector<card*>::iterator hand::get_card_iterator() {
    return _cards.begin();
}


#ifdef LAMA_SERVER
#ifndef USE_DIFFS
void hand::setup_round(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}

bool hand::add_card(card* new_card, std::string &err) {
    _cards.push_back(new_card);
    return true;
}

bool hand::remove_card(std::string card_id, card*& played_card, std::string &err) {
    played_card = nullptr;
    auto it = std::find_if(_cards.begin(), _cards.end(),
                           [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it < _cards.end()) {
        played_card = remove_card(it);
        return true;
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return false;
    }
}

#else

void hand::setup_round(object_diff &hand_diff, std::string &err) {
    // remove all cards (if any) and clear it
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_removal(0, _cards[i]->get_id());
        hand_diff.add_param_diff(cards_diff->get_name(), cards_diff);
        delete _cards[i];
    }
    _cards.clear();
}

bool hand::add_card(card* new_card, object_diff& hand_diff, std::string &err) {
    _cards.push_back(new_card);

    array_diff* arr_diff = new array_diff(this->_id + "_cards", "cards");
    arr_diff->add_insertion(_cards.size() - 1, new_card->get_id(), new_card->to_full_diff());
    hand_diff.add_param_diff(arr_diff->get_name(), arr_diff);

    return true;
}

bool hand::remove_card(std::string card_id, card*& removed_card, object_diff &hand_diff, std::string &err) {
    removed_card = nullptr;;
    auto it = std::find_if(_cards.begin(), _cards.end(),
                 [&card_id](const card* x) { return x->get_id() == card_id;});
    if (it != _cards.end()) {
        int idx = it - _cards.begin();
        removed_card = remove_card(it);

        array_diff* arr_diff = new array_diff(this->_id + "_cards", "cards");
        arr_diff->add_removal(idx, removed_card->get_id());
        hand_diff.add_param_diff(arr_diff->get_name(), arr_diff);
        return true;
    } else {
        err = "Could not play card, as the requested card was not on the player's hand.";
        return false;
    }
}

#endif
#endif



bool hand::apply_diff_specialized(const diff* state_diff) {
    const object_diff* valid_diff = dynamic_cast<const object_diff*>(state_diff);
    if (valid_diff != nullptr && valid_diff->get_id() == this->_id) {
        if (valid_diff->get_timestamp()->is_newer(this->_timestamp) && valid_diff->has_changes()) {
            bool has_changed = false;
            diff* child_diff = nullptr;
            if (valid_diff->try_get_param_diff("cards", child_diff)) {
                const array_diff* arr_diff = dynamic_cast<const array_diff*>(child_diff);
                has_changed |= array_helpers::apply_diff<card>(_cards, arr_diff);
            }
            return has_changed;
            // TODO update timestamp
        }
    }
    return false;
}

diff *hand::to_full_diff() const {
    object_diff* hand_diff = new object_diff(this->_id, this->_name);
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_insertion(i, _cards[i]->get_id(), _cards[i]->to_full_diff());
    }
    hand_diff->add_param_diff("cards", cards_diff);

    return hand_diff;
}


hand *hand::from_diff(const diff *full_hand_diff) {
    const object_diff* full_diff = dynamic_cast<const object_diff*>(full_hand_diff);
    if (full_diff != nullptr && full_diff->get_name() == "hand") {

        diff* cards_diff = nullptr;
        std::vector<card*> cards;
        if (full_diff->try_get_param_diff("cards", cards_diff)) {
            const array_diff* arr_diff = dynamic_cast<const array_diff*>(cards_diff);
            cards = array_helpers::vector_from_diff<card>(arr_diff);
        }

        return new hand(reactive_object::extract_base_params(*full_diff), cards);
    } else {
        throw LamaException("Failed to create hand from diff with name " + full_hand_diff->get_name());
    }
}



void hand::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType& allocator) const {
    reactive_object::write_into_json(json, allocator);
    json.AddMember("cards", diffable_utils::serialize_vector(_cards, allocator), allocator);
}

hand *hand::from_json(const rapidjson::Value &json) {
    if (json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        return new hand(reactive_object::extract_base_params(json), deserialized_cards);
    } else {
        throw LamaException("Could not parse hand from json. 'cards' were missing.");
    }
}



