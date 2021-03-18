//
// Created by Manuel on 25.01.2021.
//

#ifndef LAMA_DISCARD_PILE_H
#define LAMA_DISCARD_PILE_H

#include <string>
#include <vector>
#include "card.h"
#include "../player/player.h"
#include "../../reactive_state/serializable.h"
#include "../../reactive_state/reactive_objects/reactive_value.h"
#include "../../../rapidjson/include/rapidjson/document.h"

class discard_pile: public reactive_object {
private:
    std::vector<card*> _cards;

    discard_pile(base_params params);
    discard_pile(base_params params, std::vector<card*>& cards);
public:
    discard_pile();
    ~discard_pile();

// accessors
    bool can_play(const card* card);
    const card* get_top_card() const;

#ifdef LAMA_SERVER
// state update functions
    void setup_game(std::string& err);  // Clears the stack
    bool try_play(const std::string& card_id, player* player, std::string& err);
    bool try_play(card* played_card, std::string& err);
#endif

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static discard_pile* from_json(const rapidjson::Value& json);
};


#endif //LAMA_DISCARD_PILE_H
