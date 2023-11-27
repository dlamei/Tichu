#ifndef TICHU_WON_CARDS_PILE_H
#define TICHU_WON_CARDS_PILE_H

#include <vector>
#include <rapidjson/document.h>
#include "../cards/card.h"
#include "../cards/CardCombination.h"

class won_cards_pile : public serializable {

private:
    std::vector<Card> _cards;

public:
    won_cards_pile() = default;

    explicit won_cards_pile(std::vector<Card> cards);

// accessors
    [[nodiscard]] int get_score() const;

#ifdef TICHU_SERVER
    // state update functions
        void wrap_up_round();
        void add_card(const Card &card);
        void add_cards(const CardCombination &combi);
        void add_cards(const std::vector<CardCombination> &combis);
#endif

// serializable interface
    static std::optional<won_cards_pile> from_json(const rapidjson::Value &json);

    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;

};


#endif //TICHU_WON_CARDS_PILE_H
