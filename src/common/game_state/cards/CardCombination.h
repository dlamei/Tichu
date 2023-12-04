
#ifndef TICHU_CARDCOMBINATION_H
#define TICHU_CARDCOMBINATION_H

#include <algorithm>
#include <vector>
#include "../../utils.h"
#include "card.h"
#include <nlohmann/json.hpp>

enum COMBI {
    NONE, SINGLE, DOUBLE, TRIPLE, BOMB, FULLHOUSE, STRASS, TREPPE, PASS, SWITCH
};

class CardCombination {

private:
    std::vector<Card> _cards;
    int _combination_type;
    int _combination_rank;


public:
    CardCombination() = default;
    explicit CardCombination(std::vector<Card> cards);
    explicit CardCombination(Card card);

// accessors
    [[nodiscard]] int get_combination_type() const noexcept { return _combination_type; }

    [[nodiscard]] int get_combination_rank() const noexcept { return _combination_rank; }

    [[nodiscard]] const std::vector<Card> &get_cards() const noexcept { return _cards; }

// card combination functions
    int count_occurances(Card card);

    bool are_all_same_rank();

    void update_combination_type_and_rank();

    bool can_be_played_on(const std::optional<CardCombination> &other, std::string &err);

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(CardCombination, _cards, _combination_type, _combination_rank);

};


#endif //TICHU_CARDCOMBINATION_H