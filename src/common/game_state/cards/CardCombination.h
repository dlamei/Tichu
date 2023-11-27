
#ifndef TICHU_CARDCOMBINATION_H
#define TICHU_CARDCOMBINATION_H

#include <algorithm>
#include <vector>
#include <rapidjson/document.h>
#include "../../../src/common/serialization/serializable.h"
#include "card.h"

enum COMBI {
    NONE, SINGLE, DOUBLE, TRIPLE, BOMB, FULLHOUSE, STRASS, TREPPE, PASS, SWITCH
};

class CardCombination : public serializable {

private:
    std::vector<Card> _cards;
    int _combination_type;
    int _combination_rank;


public:
    CardCombination(std::vector<Card> cards);

    CardCombination(Card card);

// accessors
    [[nodiscard]] int get_combination_type() const noexcept { return _combination_type; }

    [[nodiscard]] int get_combination_rank() const noexcept { return _combination_rank; }

    [[nodiscard]] std::vector<Card> get_cards() const noexcept { return _cards; }

// card combination functions
    int count_occurances(Card card);

    bool are_all_same_rank();

    void update_combination_type_and_rank();

    bool can_be_played_on(const std::optional<CardCombination> &other, std::string &err);

// serializable interface
    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;

    static CardCombination from_json(const rapidjson::Value &json);

};


#endif //TICHU_CARDCOMBINATION_H