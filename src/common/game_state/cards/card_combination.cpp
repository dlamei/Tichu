
#include "card_combination.h"

#include <utility>

#include "../../exceptions/TichuException.h"

card_combination::card_combination(std::vector<card> cards) {
    for(card c : cards){
        _cards.push_back(c);
    }
    std::sort(_cards.begin(),_cards.end());
    update_combination_type_and_rank();
}


void card_combination::update_combination_type_and_rank() {
    //TO-DO
}

bool card_combination::can_be_played_on(const card_combination &other) const noexcept {
    return true;
    //TO-DO
}

card_combination *card_combination::from_json(const rapidjson::Value &json) {
    auto cards = vec_from_json("cards", json);

    if (!(cards)) {
        throw TichuException("Could not parse json of card_combination. Was missing 'cards'.");
    }
    return card_combination { cards.value() };
}

void card_combination::write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const {
    vec_into_json("cards", _cards, json, alloc);
}




