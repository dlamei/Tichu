/*! \class ClientMsg
    \brief Base class for client message to the server.
    
 A ClientMsg has 3 fields, the game_id, player_id and the data specific for that type of message.
*/

/*! \class ServerMsg
    \brief Base class for server communication to the client.
*/

#ifndef TICHU_MESSAGES_H
#define TICHU_MESSAGES_H

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include "utils.h"
#include "event.h"
#include "game_state/GameState.h"
#include "game_state/cards/card.h"
#include "game_state/cards/CardCombination.h"


#define VARIANT_ENUM(self_t, enum_t, value) inline enum_t get_variant_enum(const self_t &) { return enum_t::value; }

//     CLIENT -> SERVER

/**
 * \enum ClientMsgType
 * \brief Identifier for the different request types.
 * 
 * The ClientMsgType is sent with every ClientMsg to identify the type of ClientMsg
 * during deserialization on the server side.
*/
enum class ClientMsgType: int {
    join_game = 0,
    start_game,
    call_grand_tichu,
    call_small_tichu,
    dragon,
    swap,
    play_combi,
    fold,
    ping,
};

NLOHMANN_JSON_SERIALIZE_ENUM(ClientMsgType, {
    {ClientMsgType::join_game, "join_game"},
    {ClientMsgType::start_game, "start_game"},
    {ClientMsgType::call_grand_tichu, "call_grand_tichu"},
    {ClientMsgType::call_small_tichu, "call_small_tichu"},
    {ClientMsgType::dragon, "dragon"},
    {ClientMsgType::swap, "swap"},
    {ClientMsgType::play_combi, "play_combi"},
    {ClientMsgType::fold, "fold"},
    {ClientMsgType::ping, "ping"},
});

// Every client message can have a struct that holds data for this type of message
/**
 * \struct start_game_req
 * \brief Data structure for the "start_game" client message.
 * 
 */
struct start_game_req {};
VARIANT_ENUM(start_game_req, ClientMsgType, start_game)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(start_game_req)

/**
 * \struct fold_req
 * \brief Data structure for the "fold" client message.
 * 
 */
struct fold_req {};
VARIANT_ENUM(fold_req, ClientMsgType, fold)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(fold_req)

/**
 * \struct join_game_req
 * \brief Data structure for the "join_game" client message.
 * 
 * This struct is used to hold data for the "join_game" client message type. 
 * It contains the player's name and team number associated with the message.
 * 
 * \var join_game_req::player_name
 * \brief The name of the player joining the game.
 * 
 * \var join_game_req::team
 * \brief The team number to which the player is assigned.
 */
struct join_game_req { 
    std::string player_name; 
    int team;
    };
VARIANT_ENUM(join_game_req, ClientMsgType, join_game)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(join_game_req, player_name, team)

/**
 * \struct grand_tichu_req
 * \brief Data structure for the "call_grand_tichu" client message.
 * 
 * This struct is used to hold data for the "call_grand_tichu" client message type. 
 * It contains the Tichu object representing the grand tichu call.
 * 
 * \var grand_tichu_req::grand_tichu_call
 * \brief The Tichu object representing the grand tichu call.
 */
struct grand_tichu_req { Tichu grand_tichu_call; };
VARIANT_ENUM(grand_tichu_req, ClientMsgType, call_grand_tichu)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(grand_tichu_req, grand_tichu_call)

/**
 * \struct small_tichu_req
 * \brief Data structure for the "call_small_tichu" client message.
 * 
 * This struct is used to hold data for the "call_small_tichu" client message type. 
 * It contains the Tichu object representing the small tichu call.
 * 
 * \var small_tichu_req::small_tichu_call
 * \brief The Tichu object representing the small tichu call.
 */
struct small_tichu_req { Tichu small_tichu_call; };
VARIANT_ENUM(small_tichu_req, ClientMsgType, call_small_tichu)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(small_tichu_req, small_tichu_call)

/**
 * \struct dragon_req
 * \brief Data structure for the "dragon" client message.
 * 
 * This struct is used to hold data for the "dragon" client message type. It contains the UUID of the selected player for the dragon action.
 * 
 * \var dragon_req::selected_player
 * \brief The UUID of the player selected for the dragon action.
 */
struct dragon_req { UUID selected_player; };
VARIANT_ENUM(dragon_req, ClientMsgType, dragon)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(dragon_req, selected_player)

/**
 * \struct swap_req
 * \brief Data structure for the "swap" client message.
 * 
 * This struct is used to hold data for the "swap" client message type. It contains a vector of cards to be swapped.
 * 
 * \var swap_req::cards
 * \brief Vector of cards to be swapped in the "swap" action.
 */
struct swap_req { std::vector<Card> cards; };
VARIANT_ENUM(swap_req, ClientMsgType, swap)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(swap_req, cards)

/**
 * \struct play_combi_req
 * \brief Data structure for the "play_combi" client message.
 * 
 * This struct is used to hold data for the "play_combi" client message type. 
 * It contains the card combination played and an optional card as a wish.
 * 
 * \var play_combi_req::played_combi
 * \brief The card combination played in the "play_combi" action.
 * 
 * \var play_combi_req::wish
 * \brief An optional card representing the wish associated with the played combination. It may be empty.
 */
struct play_combi_req { 
    CardCombination played_combi; 
    std::optional<Card> wish;
};
VARIANT_ENUM(play_combi_req, ClientMsgType, play_combi)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(play_combi_req, played_combi, wish)

/**
 * \struct ping
 * \brief Data structure for the "ping" client message.
 * 
 */
struct ping {};
VARIANT_ENUM(ping, ClientMsgType, ping)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(ping)

// this type can hold only one of these structs at any given time
using client_msg_variant = std::variant<join_game_req, start_game_req, grand_tichu_req, small_tichu_req, 
                                        dragon_req, swap_req, play_combi_req, fold_req, ping>;

// a ClientMsg has 3 fields, the game_id, player_id and the data specific for that type of message
class ClientMsg {
private:

    // additional data depending on the msg type
    client_msg_variant _data{};
    UUID _player_id;

public:
    /**
     * \brief Constructor for ClientMsg.
     * \param player_id UUID of the player associated with the message.
     * \param var Variant holding different types of client message data.
    */
    explicit ClientMsg(UUID player_id, client_msg_variant var)
    : _data(std::move(var)), _player_id(std::move(player_id)) {}
    ClientMsg() = default;

    /**
     * \brief Gets the type of the current message.
     * \return ClientMsgType representing the type of the message.
     * 
     * The function request_to_request_type is used
    */
    [[nodiscard]] ClientMsgType get_type() const { return std::visit([&](auto&& var) { return get_variant_enum(var); }, _data); }

    //[[nodiscard]] const UUID &get_game_id() const { return this->_game_id; }

    /**
     * \brief Gets the UUID of the player associated with the message.
     * \return UUID representing the player ID.
    */
    [[nodiscard]] const UUID &get_player_id() const { return this->_player_id; }

    /**
     * \brief Gets the data specific to the given message type.
     * \return The data specific to the message type.
    */
    template<typename T>
    T get_msg_data() const {
        const int i1 = (int)get_variant_enum(T{});
        const int i2 = (int)get_type();
        ASSERT(i1 == i2, "called get_msg_data for variant {}, but found {}", i1, i2);
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ClientMsg, _data, _player_id);
};


// SERVER -> CLIENT

/**
 * \enum ServerMsgType
 * \brief Identifier for the different response types.
 * 
 * The ServerMsgType is sent with every ServerMsg to identify the type of ServerMsg
 * during deserialization on the client side.
*/
enum class ServerMsgType: int {
    req_response = 0,
    full_state,
    dragon,
    pong,
};

enum class MessageType: int {
    Info,
    Warn,
    Error,
};

/**
 * \struct server_message
 * \brief Data structure for the "req_response" server message.
 * 
 * This struct is used to hold data for the "req_response" server message type. 
 * It contains a message type and a string message.
 * 
 * \var server_message::type
 * \brief The message type indicating whether it's an informational, warning, or error message.
 * 
 * \var server_message::msg
 * \brief The string message associated with the server response.
 */
struct server_message {
    MessageType type{};
    std::string msg{};
};
VARIANT_ENUM(server_message, ServerMsgType, req_response)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(server_message, msg, type)

/**
 * \struct full_state_response
 * \brief Data structure for the "full_state" server message.
 * 
 * This struct is used to hold data for the "full_state" server message type. 
 * It contains the game state and a vector of events.
 * 
 * \var full_state_response::state
 * \brief The game state information included in the "full_state" server response.
 * 
 * \var full_state_response::events
 * \brief A vector of events associated with the game state change.
 */
struct full_state_response { 
    GameState state;
    std::vector<Event> events; 
};
VARIANT_ENUM(full_state_response, ServerMsgType, full_state)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(full_state_response, state, events)

/**
 * \struct dragon
 * \brief Data structure for the "dragon" server message.
 * 
 */
struct dragon {};
VARIANT_ENUM(dragon, ServerMsgType, dragon)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(dragon)

/**
 * \struct pong
 * \brief Data structure for the "pong" server message.
 * 
 */
struct pong {};
VARIANT_ENUM(pong, ServerMsgType, pong)
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_EMPTY(pong)

using server_msg_variant = std::variant<server_message, full_state_response, dragon, pong>;

class ServerMsg {
private:

    // additional data depending on the type of message
    server_msg_variant _data;

public:
    /**
     * \brief Constructor for ServerMsg.
     * \param var Variant holding different types of server message data.
    */
    explicit ServerMsg(server_msg_variant var)
    : _data(std::move(var)) {}
    ServerMsg() = default;

    /**
     * \brief Gets the type of the current response.
     * \return ServerMsgType representing the type of the response.
    */
    [[nodiscard]] ServerMsgType get_type() const { return std::visit(overloaded{[](const auto &r) { return get_variant_enum(r); }}, _data); }

    /**
     * \brief Gets the data specific to the given response type.
     * \tparam T Type of the response data.
     * \return The data specific to the response type.
    */
    template<typename T>
    T get_msg_data() const {
        const int i1 = (int)get_variant_enum(T{});
        const int i2 = (int)get_type();
        ASSERT(i1 == i2, "called get_msg_data for variant {}, but found {}", i1, i2);
        return std::get<T>(_data);
    }

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ServerMsg, _data);
};

#endif //TICHU_MESSAGES_H
