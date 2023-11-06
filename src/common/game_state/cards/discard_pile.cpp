
#include "discard_pile.h"
#include "../../serialization/serializable.h"
#include "../../exceptions/TichuException.h"


discard_pile::discard_pile(std::vector<card> cards):
        _cards(std::move(cards))
{ }

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
void discard_pile::setup_game(std::string &err) {
    // remove all cards (if any) and clear it
    _cards.clear();
}

bool discard_pile::try_play(const card& card_id, player &player, std::string& err) {
    auto played_card = player.get_hand().try_get_card(card_id);
    if (played_card) {
        auto card = played_card.value();
        if (can_play(card)) {
            auto local_system_card = player.remove_card(card, err);
            if (local_system_card) {
                _cards.push_back(local_system_card.value());
                return true;
            } else {
                err = "Could not play card because player does not have this card.";
            }
        } else {
            err = "The desired card with value " + std::to_string(played_card->get_value())
                  + " cannot be played on top of a card with value " + std::to_string(get_top_card()->get_value());
        }
    } else {
        err = "The player does not possess the card, which was requested to be played.";
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
    auto cards = vec_from_json<card>("cards", json);
    if (!cards) {
        throw TichuException("Could not parse draw_pile from json. 'cards' were missing.");
    }
    return discard_pile{cards.value()};
}

void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {
    vec_into_json("cards", _cards, json, alloc);
}




