#ifndef TICHU_CARD_H
#define TICHU_CARD_H

#include <string>
#include "../../../../src/common/serialization/serializable.h"
//#include "../../../../rapidjson/include/rapidjson/document.h"
#include <rapidjson/document.h>

//Macros for special cards
#define DRAGON Card(SPECIAL, RED, 25)
#define PHONIX Card(SPECIAL, GREEN, -25)
#define ONE Card(SPECIAL, SCHWARZ, 0)
#define HUND Card(SPECIAL, BLUE, 0)

enum Rank {
    SPECIAL = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING, ACE
};

enum Suit {
    GREEN = 1, RED, BLUE, SCHWARZ
    // For Special cards: RED = Dragon, GREEN = Phoenix, BLUE = Dog, SCHWARZ = One
};

class Card : public serializable {
private:
    int _rank;
    int _suit;
    int _value;

public:
    Card(int rank, int suit, int val);
    Card(int rank, int suit);

    bool operator==(const Card& other) const {
        return _rank == other._rank && _suit == other._suit && _value == other._value;
    }

    bool operator!=(const Card& other) const {
        return !(*this == other);
    }

    bool operator<(const Card& other) const {
        if(*this == DRAGON) { return false; }
        if(other == DRAGON) { return true; }
        if(*this == PHONIX) { return false; }
        if(other == PHONIX) { return true; }
        if(this->get_rank() == other.get_rank()) { 
            return this->get_suit() < other.get_suit();
        } else {
            return this->get_rank() < other.get_rank();
        }
    }

// accessors
    [[nodiscard]] int get_rank() const noexcept { return _rank; }
    [[nodiscard]] int get_suit() const noexcept { return _suit; }
    [[nodiscard]] int get_value() const noexcept { return _value; }

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static Card from_json(const rapidjson::Value& json);
};


#endif //TICHU_CARD_H
