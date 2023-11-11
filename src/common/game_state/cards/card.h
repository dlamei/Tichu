#ifndef TICHU_CARD_H
#define TICHU_CARD_H

#include <string>
#include "../../../../src/common/serialization/serializable.h"
//#include "../../../../rapidjson/include/rapidjson/document.h"
#include <rapidjson/document.h>

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

class card : public serializable {
private:
    int _rank;
    int _suit;
    int _value;

public:
    card(int rank, int suit, int val);

    bool operator==(const card& other) const {
        return _rank == other._rank && _suit == other._suit && _value == other._value;
    }

    bool operator!=(const card& other) const {
        return !(*this == other);
    }

// accessors
    [[nodiscard]] int get_rank() const noexcept { return _rank; }
    [[nodiscard]] int get_suit() const noexcept { return _suit; }
    [[nodiscard]] int get_value() const noexcept { return _value; }

// card functions
    bool can_be_played_on(const card &other) const noexcept;

// serializable interface
    void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static card from_json(const rapidjson::Value& json);
};


#endif //TICHU_CARD_H
