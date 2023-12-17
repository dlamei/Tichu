/*! \class hand
    \brief Represents the hand of a player.
    
 This class manages the cards in a player's hand, providing functions for adding, removing, and
 updating the state of the hand during a Tichu game.
*/

#ifndef TICHU_HAND_H
#define TICHU_HAND_H

#include <vector>
#include "../cards/card.h"
#include "../cards/CardCombination.h"

class hand {

private:
    std::vector<Card> _cards;

    /**
     * \brief Removes a card from the hand.
     *
     * \param pos Iterator pointing to the position of the card to be removed.
     * \return An optional representing the removed card.
     */
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

    /**
     * \brief Attempts to retrieve a card with a specific ID from the hand.
     *
     * \param card_id The ID of the card to be retrieved.
     * \return An optional representing the retrieved card (if found).
     */
    [[nodiscard]] std::optional<Card> try_get_card(const Card &card_id) const;

#ifdef TICHU_SERVER
    // state update functions
        int wrap_up_round();
        int count_occurances(Card card) const;
        bool add_card(const Card &card, std::string &err);
        void add_cards(const std::vector<Card> &cards, std::string &err);
        std::optional<Card> remove_card(const Card &card, std::string& err);
        bool remove_cards(const std::vector<Card> &cards, std::string& err);
#endif

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(hand, _cards)
};


#endif //TICHU_HAND_H
