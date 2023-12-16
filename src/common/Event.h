/*! \class Event
    \brief Represents an event.

*/

#ifndef EVENT_H
#define EVENT_H

#include "utils.h"
#include <vector>
#include <string>
#include <utility>
#include "game_state/cards/card.h"
#include "game_state/player/Player.h"

/** @enum EventType
 *  @brief Represents different types of events.
 *
 * The EventType enumeration defines various types of events that can
 * occur during a Tichu game. Each enumerator corresponds to a specific event 
 * type, providing a comprehensive set of possibilities.
*/

enum class EventType {
    GAME_START,
    PLAYER_JOINED,
    PLAYER_LEFT,
    GRAND_TICHU,
    SMALL_TICHU,
    SWAP_OUT,
    SWAP_IN,
    WISH,
    SWITCH,
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
    std::optional<UUID> player_id;   
    std::optional<Card> card;
    std::optional<int> score_A;
    std::optional<int> score_B;

    std::string to_string(const std::vector<player_ptr> &players, const UUID &me_id) const;

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Event, event_type, player_id, card, score_A, score_B)
};


#endif //EVENT_H