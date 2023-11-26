//
// Created by Manuel on 25.01.2021.
//

#ifndef TICHU_DISCARD_PILE_H
#define TICHU_DISCARD_PILE_H

#include <string>
#include <vector>
#include "card.h"
#include "card_combination.h"
#include "../player/player.h"
#include "../../serialization/serializable.h"
#include <rapidjson/document.h>

class active_pile : public serializable {
private:
    std::vector<card_combination> _active_pile;

public:
    active_pile() = default;

    explicit active_pile(std::vector<card_combination> combis);

// accessors
    [[nodiscard]] std::optional<card_combination> get_top_combi() const;

    [[nodiscard]] std::vector<card_combination> get_pile() const { return _active_pile; }

#ifdef TICHU_SERVER
    // state update functions
        void push_active_pile(const card_combination &combi);
        std::vector<card_combination> wrap_up_trick();
        void clear_cards() { _active_pile.clear(); }
#endif

// serializable interface
    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;

    static active_pile from_json(const rapidjson::Value &json);
};


#endif //TICHU_DISCARD_PILE_H
