//
// Created by Manuel on 25.01.2021.
//

#ifndef TICHU_DRAW_PILE_H
#define TICHU_DRAW_PILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../serialization/serializable.h"
#include "../../game_state/player/player.h"
#include "../../../../rapidjson/include/rapidjson/document.h"


class draw_pile : public serializable {
private:
    std::vector<Card> _cards;

    void shuffle();

public:
// constructors
    draw_pile() = default;
    explicit draw_pile(std::vector<Card> cards);


// accessors
    [[nodiscard]] bool is_empty() const noexcept;
    [[nodiscard]] int get_nof_cards() const noexcept;
    [[nodiscard]] const std::vector<Card> &get_cards() const { return _cards;}

#ifdef TICHU_SERVER
// state update functions
    void setup_game(std::string& err);  // Fills the stack with all cards of the game
    std::optional<Card> draw(player &player, std::string& err);
    std::optional<Card> remove_top(std::string& err);
#endif

// serialization
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static draw_pile from_json(const rapidjson::Value& json);
};


#endif //TICHU_DRAW_PILE_H
