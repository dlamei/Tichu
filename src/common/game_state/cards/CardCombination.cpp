
#include "CardCombination.h"

#include <utility>


CardCombination::CardCombination(std::vector<Card> cards) {
    for (Card c: cards) {
        _cards.push_back(c);
    }
    update_combination_type_and_rank();
}

CardCombination::CardCombination(Card c) {
    _cards.push_back(c);
    update_combination_type_and_rank();
}


int CardCombination::count_occurances(Card card) {
    int count = 0;
    if (card.get_rank() == SPECIAL) {
        for (Card c: _cards) {
            if (c.get_rank() == card.get_rank() && c.get_suit() == card.get_suit()) { ++count; }
        }
    } else {
        for (Card c: _cards) {
            if (c.get_rank() == card.get_rank()) { ++count; }
        }
    }
    return count;
}

bool CardCombination::are_all_same_rank() {
    int rank = _cards.at(0).get_rank();
    for (Card c: _cards) {
        if (c != PHONIX && c.get_rank() != rank) {
            return false;
        }
    }
    return true;
}

void CardCombination::update_combination_type_and_rank() {
    // making sure cards are sorted before determining type / rank
    std::sort(_cards.begin(), _cards.end());

    int size = _cards.size();
    bool has_phoenix = count_occurances(PHONIX);
    bool has_dog = count_occurances(HUND);
    bool has_dragon = count_occurances(DRAGON);

    // PASS
    if (size == 0) {
        _combination_type = PASS;
        _combination_rank = 0;
        return;
    }

    // SINGLE CARDS
    if (size == 1) {

        // DOG
        if (has_dog) {
            _combination_type = SWITCH;
            _combination_rank = 0;
            return;
        }
        // DRAGON
        if (has_dragon) {
            _combination_type = SINGLE;
            _combination_rank = 15;
            return;
        }
        // SINGLE CARD
        if (has_phoenix) {
            _combination_rank = -1;
        } else {
            _combination_rank = _cards.at(0).get_rank();
        }
        _combination_type = SINGLE;
        return;
    }

    // DOUBLE , TRIPPLE , BOMB
    if (are_all_same_rank()) {
        if (size == 2) {
            _combination_type = DOUBLE;
            _combination_rank = _cards.at(0).get_rank();
            return;
        }
        if (size == 3) {
            _combination_type = TRIPLE;
            _combination_rank = _cards.at(0).get_rank();
            return;
        }
        if (size == 4 && !has_phoenix) {
            _combination_type = BOMB;
            _combination_rank = _cards.at(0).get_rank();
            return;
        }
        _combination_type = NONE;
        _combination_rank = 0;
        return;
    }

    if (has_dog || has_dragon) {
        _combination_type = NONE;
        _combination_rank = 0;
        return;
    }

    // FULLHOUSE
    if (size == 5) {
        if (has_phoenix) {
            if (count_occurances(_cards.at(0)) == 2 && count_occurances(_cards.at(2)) == 2) {
                _combination_type = FULLHOUSE;
                _combination_rank = _cards.at(2).get_rank();
                return;
            } else if (count_occurances(_cards.at(0)) == 1 && count_occurances(_cards.at(2)) == 3) {
                _combination_type = FULLHOUSE;
                _combination_rank = _cards.at(2).get_rank();
                return;
            } else if (count_occurances(_cards.at(0)) == 3) {
                _combination_type = FULLHOUSE;
                _combination_rank = _cards.at(0).get_rank();
                return;
            }
        } else {
            if (count_occurances(_cards.at(0)) == 3) {
                if (count_occurances(_cards.at(4)) == 2) {
                    _combination_type = FULLHOUSE;
                    _combination_rank = _cards.at(0).get_rank();
                    return;
                } else {
                    _combination_type = NONE;
                    _combination_rank = 0;
                    return;
                }
            }
            if (count_occurances(_cards.at(0)) == 2) {
                if (count_occurances(_cards.at(4)) == 3) {
                    _combination_type = FULLHOUSE;
                    _combination_rank = _cards.at(4).get_rank();
                    return;
                } else {
                    _combination_type = NONE;
                    _combination_rank = 0;
                    return;
                }
            }
        }
    }

    // STRASSE
    Card previous = _cards.at(0);
    if (size >= 5) {
        if (has_phoenix) {
            int number_of_two_gaps = 0;
            bool first_loop = true;
            previous = _cards.at(0);
            for (Card card: _cards) {
                if (first_loop) {
                    first_loop = false;
                    continue;
                }
                if (card == PHONIX) { break; }
                if (card.get_rank() - previous.get_rank() == 2) {
                    ++number_of_two_gaps;
                    break;
                }
                if (card.get_rank() - previous.get_rank() == 0) {
                    number_of_two_gaps = 2;
                    break;
                }
                previous = card;
            }
            if (number_of_two_gaps <= 1) {
                _combination_type = STRASS;
                _combination_rank = _cards.at(0).get_rank();
                return;
            }
        } else {
            bool is_Strass = true;
            bool first_loop = true;
            previous = _cards.at(0);
            for (Card card: _cards) {
                if (first_loop) {
                    first_loop = false;
                    continue;
                }
                if (card.get_rank() - previous.get_rank() != 1) {
                    is_Strass = false;
                    break;
                }
                previous = card;
            }
            //check for strassenbombe
            bool all_same_suit = true;
            int first_suit = _cards.at(0).get_suit();
            for (Card card: _cards) {
                if (card.get_suit() != first_suit) {
                    all_same_suit = false;
                    break;
                }
            }
            if (all_same_suit) {
                _combination_type = BOMB;
                _combination_rank = _cards.at(0).get_rank();
                return;
            }
            if (is_Strass) {
                _combination_type = STRASS;
                _combination_rank = _cards.at(0).get_rank();
                return;
            }
        }
    }

    // TREPPE 
    bool is_Treppe = true;
    if (size % 2 == 0) {
        if (has_phoenix) {
            bool expect_gap = 0;
            bool first_loop = true;
            int mistake_count = 0;
            previous = _cards.at(0);
            for (Card card: _cards) {
                if (card == PHONIX) { break; }
                if (first_loop) {
                    first_loop = false;
                    continue;
                }
                if (card.get_rank() - previous.get_rank() != expect_gap) {
                    if (mistake_count == 0) {
                        mistake_count++;
                        previous = card;
                        continue;
                    } else {
                        is_Treppe = false;
                        break;
                    }
                }
                previous = card;
                expect_gap = !expect_gap;
            }

        } else {
            bool expect_gap = 0;
            bool first_loop = true;
            previous = _cards.at(0);
            for (const Card& card: _cards) {
                if (first_loop) {
                    first_loop = false;
                    continue;
                }
                // TODO: is this right?
                if (card.get_rank() - previous.get_rank() != expect_gap) {
                    is_Treppe = false;
                    break;
                }
                previous = card;
                expect_gap = !expect_gap;
            }

        }
        if (is_Treppe) {
            _combination_type = TREPPE;
            _combination_rank = _cards.at(0).get_rank();
            return;
        }
    }

    _combination_type = NONE;
    _combination_rank = 0;
    return;
}

bool CardCombination::can_be_played_on(const std::optional<CardCombination> &other_opt, std::string &err) {
    update_combination_type_and_rank();
    //nothing
    if (this->_combination_type == NONE) {
        err = "Invalid combination";
        return false;
    }

    //pass
    if (this->_combination_type == PASS) {
        return true;
    }

    if (!other_opt) { return true; }
    CardCombination other = other_opt.value();
    if (other.get_combination_type() == SWITCH) { return true; }

    //bombs
    if (this->_combination_type == BOMB) {
        if (other.get_combination_type() != BOMB) { return true; }
        else if (this->get_cards().size() > other.get_cards().size()) { return true; }
        else if (this->get_cards().size() == other.get_cards().size()
                 && this->get_combination_rank() > other.get_combination_rank()) { return true; }
        else {
            err = "bomb not high enough";
            return false;
        }
    }

    // single Phoenix
    if (this->_combination_type == SINGLE && this->_combination_rank == -1) {
        if (other.get_combination_type() == SINGLE) {
            if (other.get_combination_rank() != 15) {
                _combination_rank = other.get_combination_rank();
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    if (this->_combination_type == other.get_combination_type()
        && this->get_cards().size() == other.get_cards().size()) {
        if (this->_combination_rank > other.get_combination_rank()) {
            return true;
        } else {
            err = "The Combination is not high enough";
            return false;
        }

    } else {
        err = "Wrong Combination Type";
        return false;
    }
    return true;
    //TO-DO
}