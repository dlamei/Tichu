#ifndef TICHU_HAND_H
#define TICHU_HAND_H

#include <vector>
//#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../cards/card.h"
#include "../cards/CardCombination.h"

class hand : public serializable {

private:
    std::vector<Card> _cards;

    //removing card from fector bloat
    std::optional<Card> remove_card(std::vector<Card>::iterator pos);

    std::optional<Card> remove_card(int idx);

    std::optional<Card> remove_card(const Card &card);


public:
    hand() = default;

    explicit hand(std::vector<Card> cards);

    bool operator==(const hand &other) const {
        for (int i = 0; i < _cards.size(); i++) {
            if (_cards[i] != other._cards[i]) return false;
        }
        return true;
    }


// accessors
    [[nodiscard]] int get_nof_cards() const { return _cards.size(); }

    [[nodiscard]] int get_score() const;

    [[nodiscard]] const std::vector<Card> &get_cards() const { return _cards; }

    [[nodiscard]] std::optional<Card> try_get_card(const Card &card_id) const;

#ifdef TICHU_SERVER
    // state update functions
        int wrap_up_round();
        bool add_card(const Card &card, std::string &err);
        void add_cards(const std::vector<Card> &cards, std::string &err);
        std::optional<Card> remove_card(const Card &card, std::string& err);
        bool remove_cards(const std::vector<Card> &cards, std::string& err);
#endif

// serializable interface
    static std::optional<hand> from_json(const rapidjson::Value &json);

    void write_into_json(rapidjson::Value &json, rapidjson::Document::AllocatorType &allocator) const override;

};


#endif //TICHU_HAND_H
