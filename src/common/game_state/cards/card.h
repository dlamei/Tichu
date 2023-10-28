//
// Created by Manuel on 25.01.2021.
//

#ifndef TICHU_CARD_H
#define TICHU_CARD_H

#include <string>
#include "../../serialization/unique_serializable.h"
#include "../../serialization/serializable_value.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

//Macros for special cards
#define DRAGON Card(SPECIAL, RED)
#define PHONIX Card(SPECIAL, GREEN)
#define ONE Card(SPECIAL, SCHWARZ)
#define HUND Card(SPECIAL, BLUE)

enum Rank {
    SPECIAL = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
};

enum Suit {
    GREEN = 1, RED, BLUE, SCHWARZ
    // For Special cards: RED = Dragon, GREEN = Phoenix, BLUE = Dog, SCHWARZ = One
};

class card : public unique_serializable {
private:
    serializable_value<int>* _rank;
    serializable_value<int>* _suit;
    serializable_value<int>* _value;

    // from_diff constructor
    card(std::string id);
    // deserialization constructor
    card(std::string id, serializable_value<int> *rank, serializable_value<int> *suit, serializable_value<int> *val);
public:
    card(int rank, int suit, int val);
    ~card();

// accessors
    int get_rank() const noexcept;
    int get_suit() const noexcept;
    int get_value() const noexcept;

// card functions
    bool can_be_played_on(const card* const other) const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card* from_json(const rapidjson::Value& json);
};


#endif //TICHU_CARD_H
