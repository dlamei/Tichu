/*! \class ActivePile
    \brief Represents the active pile and stores all cards played during a trick.

 ###TODI
*/

#ifndef TICHU_DISCARD_PILE_H
#define TICHU_DISCARD_PILE_H

#include <string>
#include <vector>
#include "card.h"
#include "CardCombination.h"
#include "../player/Player.h"
#include <nlohmann/json.hpp>

class ActivePile {
private:
    std::vector<CardCombination> _active_pile;

public:
    ActivePile() = default;

    explicit ActivePile(std::vector<CardCombination> combis);

// accessors
    [[nodiscard]] std::optional<CardCombination> get_top_combi() const;


    [[nodiscard]] std::vector<CardCombination> get_pile() const { return _active_pile; }

    void push_active_pile(const CardCombination &combi);
    std::vector<CardCombination> wrap_up_trick();
    void clear_cards() { _active_pile.clear(); }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ActivePile, _active_pile)
};


#endif //TICHU_DISCARD_PILE_H
