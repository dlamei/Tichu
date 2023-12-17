#include "Event.h"

std::string Event::to_string(const std::vector<player_ptr> &players, const UUID &me_id) const {
    std::string event_string = "";

    // name
    std::string name;
    if(!player_id || player_id.value() == me_id) { name = "You"; }
    else { 
        for(auto player : players) {
            if(player->get_id() == player_id) { name = player->get_player_name(); }
        }
    }
    // card
    std::string card_string;
    std::string card_string_wish;
    if(card) { card_string = "a " + card.value().to_string(true); }
    else { card_string = "nothing"; }
    if(card) { card_string = "a " + card.value().to_string(false); }
    else { card_string = "nothing"; }


    switch( event_type ) {
        case EventType::GAME_START:
            event_string = "The game has started!";
            break;

        case EventType::PLAYER_LEFT:
            event_string += name + " left the game";
            break;

        case EventType::GRAND_TICHU:
            event_string += name + " called a Grand Tichu!";
            break;

        case EventType::SMALL_TICHU:
            event_string += name + " called a Tichu!";
            break;

        case EventType::SWAP_OUT:  
            event_string += "You gave " + card_string + " to " + name;
            break;

        case EventType::SWAP_IN:  
            event_string += name + " gave you " + card_string;
            break;

        case EventType::PASS:
            event_string += name + " passed";
            break;

        case EventType::SELECTION_START:
            event_string += name + " is selecting who to give the Dragon to";
            break;

        case EventType::SELECTION_END:
            event_string += "The Dragon was given to " + name;
            break;

        case EventType::BOMB:
            event_string += name + " played a Bomb!!";
            break;

        case EventType::WISH:
            event_string += name + " wished for " + card_string_wish;
            break;

        case EventType::SWITCH:
            event_string += name + " played the Dog";
            break;

        case EventType::PLAY_COMBI:
            event_string += name + " played";
            break;

        case EventType::PLAYER_FINISHED:
            event_string += name + " finished";
            break;

        case EventType::STICH_END:
            event_string += name + " won the stich!";
            break;

        case EventType::ROUND_END: 
            event_string += "| Team A +";
            event_string += std::to_string(score_A.value());
            event_string += " | Team B +";
            event_string += std::to_string(score_B.value());
            event_string += " |";
            break;

        default:
            event_string += "Unknown Event";
    }
    return event_string;
}