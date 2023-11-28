
#ifndef TICHU_DRAWPILE_H
#define TICHU_DRAWPILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../game_state/player/Player.h"
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
        void setup_game(std::string& err);  // Fills the stack with all cards of the game
        std::optional<Card> draw(Player &Player, std::string& err);
        std::optional<Card> remove_top(std::string& err);
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(DrawPile, _cards);
};


#endif //TICHU_DRAWPILE_H
