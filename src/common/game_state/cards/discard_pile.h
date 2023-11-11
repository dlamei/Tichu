#ifndef TICHU_DISCARD_PILE_H
#define TICHU_DISCARD_PILE_H

#include <string>
#include <vector>
#include "card.h"
#include "../player/player.h"
#include "../../serialization/serializable.h"
#include "../../../../rapidjson/include/rapidjson/document.h"

class discard_pile : public serializable {
private:
    std::vector<card> _cards;

public:
    discard_pile() = default;
    explicit discard_pile(std::vector<card> cards);

// accessors
    bool can_play(const card& card);
    [[nodiscard]] std::optional<card> get_top_card() const;
    [[nodiscard]] const std::vector<card> &get_cards() const { return _cards; }

#ifdef TICHU_SERVER
// state update functions
    void setup_game(std::string& err);  // Clears the stack
    bool try_play(const card& card_id, player &player, std::string& err);
    bool try_play(const card &played_card, std::string& err);
#endif

// serializable interface
    virtual void write_into_json(rapidjson::Value& json, rapidjson::Document::AllocatorType& allocator) const override;
    static discard_pile from_json(const rapidjson::Value& json);
};


#endif //TICHU_DISCARD_PILE_H
