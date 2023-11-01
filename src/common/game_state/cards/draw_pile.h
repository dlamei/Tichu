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
    UUID _id;
    std::vector<card> _cards;

    /*
     * Deserialization constructor
     */
    draw_pile(UUID id, std::vector<card> cards);

    // from_diff constructor
    draw_pile(UUID id);

// pile functions
    void shuffle();

public:
// constructors
    draw_pile();
    draw_pile(const std::vector<card>& cards);


// accessors
    bool is_empty() const noexcept;
    int get_nof_cards() const noexcept;

#ifdef TICHU_SERVER
// state update functions
    void setup_game(std::string& err);  // Fills the stack with all cards of the game
    std::optional<card> draw(player &player, std::string& err);
    std::optional<card> remove_top(std::string& err);
#endif

// serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static draw_pile from_json(const rapidjson::Value& json);
};


#endif //TICHU_DRAW_PILE_H
