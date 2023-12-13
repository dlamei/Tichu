#ifndef EVENT_H
#define EVENT_H

#include "utils.h"
#include <vector>
#include <string>
#include <utility>
#include "game_state/cards/card.h"

enum class EventType {
    GAME_START,
    PLAYER_JOINED,
    PLAYER_LEFT,
    GRAND_TICHU,
    SMALL_TICHU,
    SWAP_OUT,
    SWAP_IN,
    WISH,
    SELECTION_START,
    SELECTION_END,
    PLAY_COMBI,
    PASS,
    BOMB,
    PLAYER_FINISHED,
    STICH_END,
    ROUND_END,
};

struct Event {
    EventType event_type;
    std::optional<std::string> player_name;   
    std::optional<Card> card;
    std::optional<int> score_A;
    std::optional<int> score_B;

    std::string to_string() const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Event, event_type, player_name, card, score_A, score_B)
};


#endif //EVENT_H