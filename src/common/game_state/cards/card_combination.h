
#ifndef TICHU_CARD_COMBINATION_H
#define TICHU_CARD_COMBINATION_H

#include <algorithm>
#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../../../src/common/serialization/serializable.h"
#include "card.h"

class card_combination : public serializable {

enum COMBI{
    NONE, SINGLE, DOUBLE, TRIPLE, BOMB, FULLHOUSE, STRASS, TREPPE, PASS, SWITCH
};

private:
    std::vector<Card> _cards;
    int _combination_type;
    int _combination_rank;


public:
    card_combination(std::vector<Card> cards);
    card_combination(Card card);
    
// accessors
    [[nodiscard]] int get_combination_type() const noexcept { return _combination_type; }
    [[nodiscard]] int get_combination_rank() const noexcept { return _combination_rank; }
    [[nodiscard]] std::vector<Card> get_cards() const noexcept { return _cards; }

// card combination functions
    int count_occurances(Card card);
    bool are_all_same_rank();
    void update_combination_type_and_rank();
    bool can_be_played_on(const std::optional<card_combination> &other, std::string &err);

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card_combination from_json(const rapidjson::Value& json);

};


#endif //TICHU_CARD_COMBINATION_H