#include "DrawPile.h"

#include <utility>
#include <random>


DrawPile::DrawPile(std::vector<Card> cards)
        : _cards(std::move(cards)) {}


void DrawPile::shuffle() {
    std::shuffle(_cards.begin(), _cards.end(), std::mt19937(std::random_device()()));
}

bool DrawPile::is_empty() const noexcept {
    return _cards.empty();
}

int DrawPile::get_nof_cards() const noexcept {
    return _cards.size();
}


#ifdef TICHU_SERVER
void DrawPile::setup_game(std::string &err) {
    // remove all cards (if any) and add the change to the "cards" array_diff
    _cards.clear();

    // add a fresh set of cards
    for (int card_rank = 1; card_rank <= 14; card_rank++) {
        for (int card_suit = 1; card_suit <= 4; card_suit++) {
            _cards.push_back(Card(card_rank, card_suit));
        }
    }
    // shuffle them
    this->shuffle();
}

std::optional<Card> DrawPile::draw(Player &Player, std::string &err)  {
    if (!_cards.empty()) {
        auto drawn_card = _cards.back();
        if (Player.add_card_to_hand(drawn_card, err)) {
            _cards.pop_back();
            return drawn_card;
        }
    } else {
        err = "Could not draw card because draw pile is empty.";
    }
    return {};
}

std::optional<Card> DrawPile::remove_top(std::string& err) {
    if (!_cards.empty()) {
        auto drawn_card = _cards.back();
        _cards.pop_back();
        return drawn_card;
    } else {
        err = "Could not draw card because draw pile is empty.";
        return {};
    }
}

#endif

