/*! \class WonCardsPile
    \brief Represents the pile of cards a player accumulates throughout a round.
    
*/

#ifndef TICHU_WON_CARDS_PILE_H
#define TICHU_WON_CARDS_PILE_H

#include <vector>
#include "card.h"
#include "card_combination.h"

class WonCardsPile {

private:
    std::vector<Card> _cards;

public:
    WonCardsPile() = default;

    explicit WonCardsPile(std::vector<Card> cards);

// accessors
    [[nodiscard]] int get_score() const;
    [[nodiscard]] int get_nof_cards() const { return (int)_cards.size(); }

#ifdef TICHU_SERVER
    // state update functions
        void wrap_up_round();
        void add_card(const Card &card);
        void add_cards(const CardCombination &combi);
        void add_cards(const std::vector<CardCombination> &combis);
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(WonCardsPile, _cards)
};


#endif //TICHU_WON_CARDS_PILE_H
