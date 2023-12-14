
#include "card.h"

#include <utility>

Card::Card(int rank, int suit, int val) : _rank(rank), _suit(suit), _value(val) {}

Card::Card(int rank, int suit) : _rank(rank), _suit(suit) {
    int value = 0;
    if (rank == 5) { value = 5; }
    else if (rank == 10 || rank == 13) { value = 10; }
    else if (rank == 1 && suit == 1) { value = -25; }
    else if (rank == 1 && suit == 2) { value = 25; }
    _value = value;
}

std::string Card::to_string() const {
    if(_rank == SPECIAL) {
        switch(_suit) {
            case GREEN: 
                return "[Phoenix]";
            case RED: 
                return "[Dragon]";
            case BLUE: 
                return "[Dog]";
            case SCHWARZ:
                return "[Majong]";
        }
    }

    std::string card_string = "[";

    switch(_suit) {
        case GREEN:
            card_string += "Green";
            break;
        case RED:
            card_string += "Red";
            break;
        case BLUE:
            card_string += "Blue";
            break;
        case SCHWARZ:
            card_string += "Black";
            break;
    }

    card_string += " ";

    switch(_rank) {
        case TWO:
            card_string += "Two";
            break;
        case THREE:
            card_string += "Three";
            break;
        case FOUR:
            card_string += "Four";
            break;
        case FIVE:
            card_string += "Five";
            break;
        case SIX:
            card_string += "Six";
            break;
        case SEVEN:
            card_string += "Seven";
            break;
        case EIGHT:
            card_string += "Eight";
            break;
        case NINE: 
            card_string += "Nine";
            break;
        case TEN:
            card_string += "Ten";
            break;
        case JACK:
            card_string += "Jack";
            break;
        case QUEEN:
            card_string += "Queen";
            break;
        case KING:
            card_string += "King";
            break;
        case ACE:
            card_string += "Ace";
            break;
    }
    card_string += "]";
    return card_string;

}
