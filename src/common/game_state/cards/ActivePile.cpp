
#include "ActivePile.h"


ActivePile::ActivePile(std::vector<CardCombination> combis) :
        _active_pile(std::move(combis)) {}

std::optional<CardCombination> ActivePile::get_top_combi() const {
    if (!_active_pile.empty()) {
        return _active_pile.back();
    } else {
        return {};
    }
}

void ActivePile::push_active_pile(const CardCombination &combi) {
    _active_pile.push_back(combi);
}

std::vector<CardCombination> ActivePile::wrap_up_trick() {
    auto temp_pile = _active_pile;
    clear_cards();
    return temp_pile;
}