#include "Event.h"

std::string Event::to_string() const {
    std::string event_string = "";

    switch( event_type ) {
        case EventType::GAME_START:
            event_string = "The game has started!";
            break;

        case EventType::PLAYER_JOINED:
            event_string += player_name.value_or("You");
            event_string += " joined the game";
            break;

        case EventType::PLAYER_LEFT:
            event_string += player_name.value_or("You");
            event_string += " left the game";
            break;

        case EventType::GRAND_TICHU:
            event_string += player_name.value_or("You");
            event_string += " called a Grand Tichu!";
            break;

        case EventType::SMALL_TICHU:
            event_string += player_name.value_or("You");
            event_string += " called a Tichu!";
            break;

        case EventType::SWAP_OUT:  
            event_string += "You gave a ";
            event_string += card.value().to_string();
            event_string += " to ";
            event_string += player_name.value_or("You");
            break;

        case EventType::SWAP_IN:  
            event_string += player_name.value_or("You");
            event_string += " gave you a ";
            event_string += card.value().to_string();
            break;

        case EventType::PASS:
            event_string += player_name.value_or("You");
            event_string += " passed";
            break;

        case EventType::SELECTION_START:
            event_string += player_name.value_or("You");
            event_string += " is selecting who to give the Dragon to";
            break;

        case EventType::SELECTION_END:
            event_string += "The Dragon was given to ";
            event_string += player_name.value_or("You");
            break;

        case EventType::BOMB:
            event_string += player_name.value_or("You");
            event_string += " played a Bomb!!";
            break;

        case EventType::WISH:
            event_string += player_name.value_or("You");
            event_string += " wished for ";
            if(card) {
                event_string += "a ";
                event_string += card.value().to_string();
            } else {
                event_string += "nothing";
            }
            break;
        case EventType::PLAY_COMBI:
            event_string += player_name.value_or("You");
            event_string += " played";
            break;

        case EventType::PLAYER_FINISHED:
            event_string += player_name.value_or("You");
            event_string += " finished";
            break;

        case EventType::STICH_END:
            event_string += player_name.value_or("You");
            event_string += " won the stich!";
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