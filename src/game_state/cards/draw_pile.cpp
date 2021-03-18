//
// Created by Manuel on 25.01.2021.
//

#include "draw_pile.h"

#include <string>
#include <iostream>

#include "../../common/utils/uuid_generator.h"
#include "../../reactive_state/diffable_utils.h"
#include "../../reactive_state/diffs/array_elem_diff.h"
#include "../../reactive_state/diffs/array_diff.h"
#include "../../reactive_state/array_helpers.h"
#include "../../common/utils/LamaException.h"


// deserialization constructor
draw_pile::draw_pile(base_params params, std::vector<card*> &cards)
        : reactive_object(params),
          _cards(cards)
{ }

// from_diff constructor
draw_pile::draw_pile(reactive_object::base_params params) : reactive_object(params) { }


draw_pile::draw_pile(std::vector<card*> &cards)
        : reactive_object("draw_pile"), _cards(cards)
{ }


draw_pile::draw_pile() : reactive_object("draw_pile") { }

draw_pile::~draw_pile() {
    for (card* & _card : _cards) {
        delete _card;
    }
    _cards.clear();
}


void draw_pile::shuffle() {
    std::shuffle(_cards.begin(), _cards.end(), std::mt19937(std::random_device()()));
}

bool draw_pile::is_empty() const noexcept  {
    return _cards.empty();
}

int draw_pile::get_nof_cards() const noexcept  {
    return _cards.size();
}


#ifdef LAMA_SERVER
#ifndef USE_DIFFS
void draw_pile::setup_game(std::string &err) {
    // remove all cards (if any) and add the change to the "cards" array_diff
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();

    // add a fresh set of cards
    for (int card_value = 1; card_value <= 7; card_value++) {
        for (int i = 0; i < 8; i ++) {
            _cards.push_back(new card(card_value));
        }
    }
    // shuffle them
    this->shuffle();
}

bool draw_pile::draw(player* player, card*& drawn_card, std::string& err)  {
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        if (player->add_card(drawn_card, err)) {
            _cards.pop_back();
            return true;
        } else {
            drawn_card = nullptr;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return false;
}

card* draw_pile::remove_top(std::string& err) {
    card* drawn_card = nullptr;
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        _cards.pop_back();
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return drawn_card;
}


#else

void draw_pile::setup_game(object_diff &pile_diff, std::string &err) {
    // create array_diff for "cards"
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");

    // remove all cards (if any) and add the change to the "cards" array_diff
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_removal(0, _cards[i]->get_id());
        delete _cards[i];
    }
    _cards.clear();

    // add fresh set of cards
    for (int card_value = 1; card_value <= 7; card_value++) {
        for (int i = 0; i < 8; i ++) {
            _cards.push_back(new card(card_value));
        }
    }
    // shuffle them
    this->shuffle();

    // store diffs to "cards" array_diff
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_insertion(i, _cards[i]->get_id(), _cards[i]->to_full_diff());
    }
    // add array_diff to pile_diff
    pile_diff.add_param_diff(cards_diff->get_name(), cards_diff);
}


bool draw_pile::draw(player* player, card*& drawn_card, object_diff& player_diff, object_diff& pile_diff, std::string& err)  {
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        if (player->add_card(drawn_card, player_diff, err)) {
            _cards.pop_back();

            array_diff* arr_diff = new array_diff(this->_id + "_cards", "cards");
            arr_diff->add_removal(_cards.size(), drawn_card->get_id());
            pile_diff.add_param_diff(arr_diff->get_name(), arr_diff);
            return true;
        } else {
            drawn_card = nullptr;
            return false;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return false;
}

card* draw_pile::remove_top(object_diff& pile_diff, std::string& err) {
    card* drawn_card = nullptr;
    if (!_cards.empty()) {
        drawn_card = _cards.back();
        _cards.pop_back();
        array_diff* arr_diff = new array_diff(this->_id + "_cards", "cards");
        arr_diff->add_removal(_cards.size(), drawn_card->get_id());
        pile_diff.add_param_diff(arr_diff->get_name(), arr_diff);
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return drawn_card;
}

#endif

#endif



bool draw_pile::apply_diff_specialized(const diff* const state_diff) {
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

diff *draw_pile::to_full_diff() const {
    object_diff* pile_diff = new object_diff(this->_id, this->_name);
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_insertion(i, _cards[i]->get_id(), _cards[i]->to_full_diff());
    }
    pile_diff->add_param_diff("cards", cards_diff);

    return pile_diff;
}


draw_pile *draw_pile::from_diff(const diff *full_pile_diff) {
    const object_diff* full_diff = dynamic_cast<const object_diff*>(full_pile_diff);
    if (full_diff != nullptr && full_diff->get_name() == "draw_pile") {
        diff* cards_diff = nullptr;
        std::vector<card*> cards;
        if (full_diff->try_get_param_diff("cards", cards_diff)) {
            const array_diff* arr_diff = dynamic_cast<const array_diff*>(cards_diff);
            cards = array_helpers::vector_from_diff<card>(arr_diff);
        }

        return new draw_pile(reactive_object::extract_base_params(*full_diff), cards);
    } else {
        throw LamaException("Failed to create draw_pile from diff with name " + full_pile_diff->get_name());
    }
}


void draw_pile::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    reactive_object::write_into_json(json, allocator);
    json.AddMember("cards", diffable_utils::serialize_vector(_cards, allocator), allocator);
}



draw_pile *draw_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        base_params params = reactive_object::extract_base_params(json);
        return new draw_pile(params, deserialized_cards);
    } else {
        throw LamaException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}
