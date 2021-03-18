//
// Created by Manuel on 25.01.2021.
//

#include "discard_pile.h"
#include "../../reactive_state/array_helpers.h"
#include "../../reactive_state/diffable_utils.h"
#include "../../reactive_state/diffs/array_diff.h"
#include "../../common/utils/uuid_generator.h"
#include "../../common/utils/LamaException.h"


discard_pile::discard_pile(reactive_object::base_params params) : reactive_object(params) { }

discard_pile::discard_pile(base_params params, std::vector<card *> &cards):
        reactive_object(params),
        _cards(cards)
{ }

discard_pile::discard_pile() : reactive_object("discard_pile") { }

discard_pile::~discard_pile() {
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}


bool discard_pile::can_play(const card *card)  {
    return _cards.empty() || card->can_be_played_on(_cards.back());
}

const card* discard_pile::get_top_card() const  {
    if (_cards.size() > 0) {
        return _cards.back();
    } else {
        return nullptr;
    }
}

#ifdef LAMA_SERVER
#ifndef USE_DIFFS

void discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    for (int i = 0; i < _cards.size(); i++) {
        delete _cards[i];
    }
    _cards.clear();
}

bool discard_pile::try_play(const std::string& card_id, player* player, std::string& err) {
    card* played_card = nullptr;
    if (player->get_hand()->try_get_card(card_id, played_card)) {
        if (can_play(played_card)) {
            card* local_system_card;
            if (player->remove_card(played_card->get_id(), local_system_card, err)) {
                _cards.push_back(local_system_card);
                return true;
            } else {
                err = "Could not play card " + played_card->get_id() + " because player does not have this card.";
            }
        } else {
            err = "The desired card with value " + std::to_string(played_card->get_value())
                  + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
        }
    } else {
        err = "The player does not possess the card " + card_id + ", which was requested to be played.";
    }
    return false;
}

bool discard_pile::try_play(card* played_card, std::string& err) {
    if (can_play(played_card)) {
        _cards.push_back(played_card);
        return true;
    } else {
        err = "The desired card with value " + std::to_string(played_card->get_value())
              + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
    }
    return false;
}

#else

void discard_pile::setup_game(object_diff &pile_diff, std::string &err) {
    // remove all cards (if any) and clear it
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_removal(0, _cards[i]->get_id());
        delete _cards[i];
    }
    pile_diff.add_param_diff(cards_diff->get_name(), cards_diff);
    _cards.clear();
}

bool discard_pile::try_play(const std::string& card_id, player* player, object_diff& pile_diff, object_diff& player_diff, std::string& err) {
    card* played_card = nullptr;
    if (player->get_hand()->try_get_card(card_id, played_card)) {
        if (can_play(played_card)) {
            card* local_system_card;
            if (player->remove_card(played_card->get_id(), local_system_card, player_diff, err)) {
                _cards.push_back(local_system_card);

                array_diff* cards_diff = new array_diff(this->_id, "cards");
                cards_diff->add_insertion(_cards.size() - 1, played_card->get_id(), local_system_card->to_full_diff());
                pile_diff.add_param_diff(cards_diff->get_name(), cards_diff);

                return true;
            } else {
                err = "Could not play card " + played_card->get_id() + " because player does not have this card.";
            }
        } else {
            err = "The desired card with value " + std::to_string(played_card->get_value())
                  + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
        }
    } else {
        err = "The player does not possess the card " + card_id + ", which was requested to be played.";
    }
    return false;
}

bool discard_pile::try_play(card* played_card, object_diff& pile_diff, std::string& err) {
    if (can_play(played_card)) {
        _cards.push_back(played_card);

        array_diff* cards_diff = new array_diff(this->_id, "cards");
        cards_diff->add_insertion(_cards.size() - 1, played_card->get_id(), played_card->to_full_diff());
        pile_diff.add_param_diff(cards_diff->get_name(), cards_diff);

        return true;
    } else {
        err = "The desired card with value " + std::to_string(played_card->get_value())
              + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
    }
    return false;
}

#endif
#endif

bool discard_pile::apply_diff_specialized(const diff* state_diff) {
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

diff *discard_pile::to_full_diff() const {
    object_diff* pile_diff = new object_diff(this->_id, this->_name);
    array_diff* cards_diff = new array_diff(this->_id + "_cards", "cards");
    for (int i = 0; i < _cards.size(); i++) {
        cards_diff->add_insertion(i, _cards[i]->get_id(), _cards[i]->to_full_diff());
    }
    pile_diff->add_param_diff("cards", cards_diff);

    return pile_diff;
}

discard_pile *discard_pile::from_diff(const diff *full_pile_diff) {
    const object_diff* full_diff = dynamic_cast<const object_diff*>(full_pile_diff);
    if (full_diff != nullptr && full_diff->get_name() == "discard_pile") {

        diff* cards_diff = nullptr;
        std::vector<card*> cards;
        if (full_diff->try_get_param_diff("cards", cards_diff)) {
            const array_diff* arr_diff = dynamic_cast<const array_diff*>(cards_diff);
            cards = array_helpers::vector_from_diff<card>(arr_diff);
        }

        return new discard_pile(reactive_object::extract_base_params(*full_diff), cards);
    } else {
        throw LamaException("Failed to create discard_pile from diff with name " + full_pile_diff->get_name());
    }
}



discard_pile *discard_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("cards")) {
        std::vector<card*> deserialized_cards = std::vector<card*>();
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        base_params params = reactive_object::extract_base_params(json);
        return new discard_pile(params, deserialized_cards);
    } else {
        throw LamaException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}



void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    reactive_object::write_into_json(json, allocator);
    json.AddMember("cards", diffable_utils::serialize_vector(_cards, allocator), allocator);
}





