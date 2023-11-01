//
// Created by Manuel on 25.01.2021.
//

#include "discard_pile.h"
#include "../../serialization/serializable.h"
#include "../../serialization/vector_utils.h"
#include "../../exceptions/TichuException.h"


discard_pile::discard_pile(UUID id) : _id(id) { }

discard_pile::discard_pile(UUID id, const std::vector<card> &cards):
        _id(id),
        _cards(cards)
{ }

discard_pile::discard_pile() : _id(UUID::create()) { }

bool discard_pile::can_play(const card &card)  {
    return _cards.empty() || card.can_be_played_on(_cards.back());
}

std::optional<card> discard_pile::get_top_card() const  {
    if (!_cards.empty()) {
        return _cards.back();
    } else {
        return {};
    }
}

#ifdef TICHU_SERVER
//TODO: what happenes here? (try_get_cards)
void discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    _cards.clear();
}

bool discard_pile::try_play(const UUID& card_id, player &player, std::string& err) {
    auto played_card = player.get_hand().try_get_card(card_id);
    if (played_card) {
        if (can_play(played_card.value())) {
            auto local_system_card = player.remove_card(played_card.value().get_id(), err);
            if (local_system_card) {
                _cards.push_back(local_system_card.value());
                return true;
            } else {
                err = "Could not play card " + played_card->get_id().string() + " because player does not have this card.";
            }
        } else {
            err = "The desired card with value " + std::to_string(played_card->get_value())
                  + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
        }
    } else {
        err = "The player does not possess the card " + card_id.string() + ", which was requested to be played.";
    }
    return false;
}

bool discard_pile::try_play(const card &played_card, std::string& err) {
    if (can_play(played_card)) {
        _cards.push_back(played_card);
        return true;
    } else {
        err = "The desired card with value " + std::to_string(played_card.get_value())
              + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
    }
    return false;
}
#endif


discard_pile discard_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card> deserialized_cards {};
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        auto id = UUID::from_json(json);
        return discard_pile{id.value(), deserialized_cards};
    } else {
        throw TichuException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}

void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    _id.write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}




