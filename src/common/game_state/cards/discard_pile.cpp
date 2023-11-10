
#include "discard_pile.h"
#include "../../serialization/serializable.h"
#include "../../exceptions/TichuException.h"


discard_pile::discard_pile(std::vector<card_combination> combis):
        _active_pile(std::move(combis))
{ }

std::optional<card_combination> discard_pile::get_top_combi() const  {
    if (!_active_pile.empty()) {
        return _active_pile.back();
    } else {
        return {};
    }
}

#ifdef TICHU_SERVER
    void discard_pile::push_active_pile(const card_combination &combi) {
        _active_pile.push_back(combi);
    }

    std::vector<card_combination> discard_pile::wrap_up_trick() {
        auto temp_pile = _active_pile;
        clear_cards();
        return temp_pile;
    }
#endif


discard_pile discard_pile::from_json(const rapidjson::Value &json) {
    auto combis = vec_from_json<card_combination>("combis", json);
    if (!combis) {
        throw TichuException("Could not parse discard_pile from json. 'combis' were missing.");
    }
    return discard_pile{combis.value()};
}

void discard_pile::write_into_json(rapidjson::Value &json,
                                   rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator> &alloc) const {
    vec_into_json("combis", _active_pile, json, alloc);
}




