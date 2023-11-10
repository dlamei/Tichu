#ifndef TICHU_HAND_H
#define TICHU_HAND_H

#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../cards/card.h"
#include "../cards/card_combination.h"

class hand : public serializable {

private:
    std::vector<card> _cards;

    std::optional<card> remove_card(std::vector<card>::iterator pos);
    std::optional<card> remove_card(int idx);
    std::optional<card> remove_card(const card& card);

public:
    hand() = default;
    explicit hand(std::vector<card> cards);

    bool operator==(const hand& other) const {
        for (int i = 0; i < _cards.size(); i++) {
            if (_cards[i] != other._cards[i]) return false;
        }
        return true;
    }

// serializable interface
    static std::optional<hand> from_json(const rapidjson::Value& json);
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;

// accessors
    [[nodiscard]] int get_nof_cards() const { return _cards.size(); }
    [[nodiscard]] int get_score() const;
    [[nodiscard]] const std::vector<card> &get_cards() const { return _cards; }
    [[nodiscard]] std::optional<card> try_get_card(const card &card_id) const;

#ifdef TICHU_SERVER
// state update functions
    void setup_round(std::string& err);
    bool add_card(const card &card, std::string &err);
    std::optional<card> remove_card(const card &card, std::string& err);
#endif

    std::vector<card>::iterator get_card_iterator();
};


#endif //TICHU_HAND_H
