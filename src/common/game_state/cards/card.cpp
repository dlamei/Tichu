
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