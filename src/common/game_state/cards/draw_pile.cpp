//
// Created by Manuel on 25.01.2021.
//

#include "draw_pile.h"

#include <utility>
#include <random>


#include "../../exceptions/TichuException.h"


draw_pile::draw_pile(std::vector<Card> cards)
        : _cards(std::move(cards))
{ }



void draw_pile::shuffle() {
    std::shuffle(_cards.begin(), _cards.end(), std::mt19937(std::random_device()()));
}

bool draw_pile::is_empty() const noexcept  {
    return _cards.empty();
}

int draw_pile::get_nof_cards() const noexcept  {
    return _cards.size();
}


#ifdef TICHU_SERVER
void draw_pile::setup_game(std::string &err) {
    // remove all cards (if any) and add the change to the "cards" array_diff
    _cards.clear();

    // add a fresh set of cards
    for (int card_rank = 1; card_rank <= 14; card_rank++) {
        for (int card_suit = 1; card_suit <= 4; card_suit++) {
            int card_value = 0;
            if(card_rank == 5) { card_value = 5; }
            else if(card_rank == 10 || card_rank == 13) { card_value = 10;}
            else if(card_rank == 1 && card_suit == 1) { card_value = -25; }
            else if(card_rank == 1 && card_suit == 2) { card_value = 25; }
            _cards.push_back(Card(card_rank, card_suit, card_value));
        }
    }
    // shuffle them
    this->shuffle();
}

std::optional<Card> draw_pile::draw(player &player, std::string &err)  {
    if (!_cards.empty()) {
        auto drawn_card = _cards.back();
        if (player.add_card_to_hand(drawn_card, err)) {
            _cards.pop_back();
            return drawn_card;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return {};
}

std::optional<Card> draw_pile::remove_top(std::string& err) {
    if (!_cards.empty()) {
        auto drawn_card = _cards.back();
        _cards.pop_back();
        return drawn_card;
    } else {
        err = "Could not draw card because draw pile is empty.";
        return {};
    }
}

#endif


void draw_pile::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {
    vec_into_json("cards", _cards, json, alloc);
}


draw_pile draw_pile::from_json(const rapidjson::Value &json) {
    auto cards = vec_from_json<Card>("cards", json);
    if (!cards) {
        throw TichuException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }

    return draw_pile(cards.value());
}
