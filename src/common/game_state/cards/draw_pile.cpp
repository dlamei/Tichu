//
// Created by Manuel on 25.01.2021.
//

#include "draw_pile.h"


#include "../../serialization/vector_utils.h"
#include "../../exceptions/TichuException.h"


// deserialization constructor
draw_pile::draw_pile(UUID id, std::vector<card> cards)
        : _id(id),
          _cards(std::move(cards))
{ }

// from_diff constructor
draw_pile::draw_pile(UUID id) : _id(id) { }


draw_pile::draw_pile(const std::vector<card> &cards)
        : _id(UUID::create()), _cards(cards)
{ }


draw_pile::draw_pile() : _id(UUID::create()) { }

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
            if(card_rank == 10 && card_rank == 13) { card_value = 10;}
            if(card_rank == 1 && card_suit == 1) { card_value = -25; }
            if(card_rank == 1 && card_suit == 2) { card_value = 25; }
            _cards.push_back(card(card_rank, card_suit, card_value));
        }
    }
    // shuffle them
    this->shuffle();
}

std::optional<card> draw_pile::draw(player &player, std::string &err)  {
    if (!_cards.empty()) {
        auto drawn_card = _cards.back();
        if (player.add_card(drawn_card, err)) {
            _cards.pop_back();
            return drawn_card;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return {};
}

std::optional<card> draw_pile::remove_top(std::string& err) {
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


void draw_pile::write_into_json(rapidjson::Value &json, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &allocator) const {
    _id.write_into_json(json, allocator);
    json.AddMember("cards", vector_utils::serialize_vector(_cards, allocator), allocator);
}


draw_pile draw_pile::from_json(const rapidjson::Value &json) {
    if (json.HasMember("id") && json.HasMember("cards")) {
        std::vector<card> deserialized_cards = {};
        for (auto &serialized_card : json["cards"].GetArray()) {
            deserialized_cards.push_back(card::from_json(serialized_card.GetObject()));
        }
        auto id = UUID::from_json(json);
        return draw_pile(id.value(), deserialized_cards);
    } else {
        throw TichuException("Could not parse draw_pile from json. 'id' or 'cards' were missing.");
    }
}
