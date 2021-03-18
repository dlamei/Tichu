//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_DRAW_PILE_H
#define LAMA_DRAW_PILE_H

#include "card.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../../reactive_state/serializable.h"
#include "../../reactive_state/reactive_objects/reactive_object.h"
#include "../../reactive_state/reactive_objects/reactive_value.h"
#include "../../game_state/player/player.h"
#include "../../../rapidjson/include/rapidjson/document.h"


class draw_pile : public reactive_object {
private:
    std::vector<card*> _cards;

    /*
     * Deserialization constructor
     */
    draw_pile(base_params params, std::vector<card*>& cards);

    // from_diff constructor
    draw_pile(base_params params);

// pile functions
    void shuffle();

public:
// constructors
    draw_pile();
    draw_pile(std::vector<card*>& cards);
    ~draw_pile();


// accessors
    bool is_empty() const noexcept;
    int get_nof_cards() const noexcept;

#ifdef LAMA_SERVER
#ifndef USE_DIFFS
// state update functions
    void setup_game(std::string& err);  // Fills the stack with all cards of the game
    bool draw(player* player, card*& drawn_card, std::string& err);
    card* remove_top(std::string& err);
#else
// state update functions with diff
    void setup_game(object_diff& pile_diff, std::string& err);  // Fills the stack with all cards of the game
    bool draw(player* player, card*& drawn_card, object_diff& player_diff, object_diff& pile_diff, std::string& err);
    card* remove_top(object_diff& pile_diff, std::string& err);
#endif
#endif

// serialization
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static draw_pile* from_json(const rapidjson::Value& json);
};


#endif //LAMA_DRAW_PILE_H
