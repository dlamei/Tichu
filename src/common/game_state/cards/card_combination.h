
#ifndef TICHU_CARD_COMBINATION_H
#define TICHU_CARD_COMBINATION_H

#include <algorithm>
#include <vector>
#include "../../../../rapidjson/include/rapidjson/document.h"
#include "../../../../src/common/serialization/serializable.h"
#include "card.h"

class card_combination : public serializable {

enum COMBI{
    NONE, SINGLE, DOUBLE, TRIPLE, BOMB, FULLHOUSE, STRASS, TREPPE
};

private:
    std::vector<card> _cards;
    int _combination_type;
    int _combination_rank;


public:
    card_combination(std::vector<card> cards);
    
// accessors
    [[nodiscard]] int get_combination_type() const noexcept { return _combination_type; }
    [[nodiscard]] int get_combination_rank() const noexcept { return _combination_rank; }
    [[nodiscard]] std::vector<card> get_cards() const noexcept { return _cards; }

// card combination functions
    void update_combination_type_and_rank();
    bool can_be_played_on(const card_combination &other) const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card_combination from_json(const rapidjson::Value& json);

};


#endif //TICHU_CARD_COMBINATION_H