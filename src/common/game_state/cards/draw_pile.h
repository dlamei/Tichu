/*! \class DrawPile
    \brief Represents a draw pile and stores cards
    
 The DrawPile is needed to set up a round and distribute the cards to the players randomly. As soon as
 all 4 players have 14 cards, the pile must be empty and the object has no further function until the 
 next round is started.
*/

#ifndef TICHU_DRAW_PILE_H
#define TICHU_DRAW_PILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../game_state/player/player.h"
#include <nlohmann/json.hpp>


class DrawPile {
private:
    std::vector<Card> _cards;

    void shuffle();

public:
// constructors
    DrawPile() = default;

    explicit DrawPile(std::vector<Card> cards);


// accessors
    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] int get_nof_cards() const noexcept;
    [[nodiscard]] const std::vector<Card> &get_cards() const { return _cards; }

#ifdef TICHU_SERVER
    // state update functions
        /**
         * Fills the stack with all cards of the game
        */
        void setup_game(std::string& err); 
        std::optional<Card> draw(Player &Player, std::string& err);
        std::optional<Card> remove_top(std::string& err);
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DrawPile, _cards);
};


#endif //TICHU_DRAW_PILE_H
