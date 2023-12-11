
#ifndef TICHU_REQUEST_HANDLER_CPP
#define TICHU_REQUEST_HANDLER_CPP

#include "request_handler.h"

#include "player_manager.h"
#include "game_instance_manager.h"
#include "GameInstance.h"


namespace request_handler {
    std::optional<ServerMsg> handle_request(const ClientMsg &req) {
        std::string err;
        const auto &player_id = req.get_player_id();
        ClientMsgType type = req.get_type();

        // Switch behavior according to request type
        switch (type) {

            // ##################### JOIN GAME #####################  //
            case ClientMsgType::join_game: {
                std::string player_name = req.get_msg_data<join_game_req>().player_name;
                Team team = (Team)(req.get_msg_data<join_game_req>().team);

                // Create new Player or get existing one with that name
                auto player = player_manager::add_or_get_player(player_name, player_id, team);

                // join any game    
                auto game_instance_ptr = game_instance_manager::try_add_player_to_any_game(player, err);
                if (game_instance_ptr) {
                    // game_instance_ptr got updated to the joined game
                    // return full GameState attached
                    return ServerMsg(full_state_response{game_instance_ptr.value()->get_game_state()});
                } else {
                    // failed to find game to join
                    auto resp = server_message{MessageType::Error, err };
                    return ServerMsg(resp);
                }
            }

                // ##################### START GAME ##################### //
            case ClientMsgType::start_game: {
                auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
                if (game_and_player) {
                    auto [player, game_instance] = game_and_player.value();
                    if (game_instance->start_game(player, err)) {
                        return {};
                    }
                }
                auto resp = server_message{MessageType::Error, err};
                return ServerMsg(resp);
            }

                // ##################### GRAND TICHU ##################### //
            case ClientMsgType::call_grand_tichu: {
                auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
                if (game_and_player) {
                    auto [player, game_instance] = game_and_player.value();
                    auto grand_tichu_call = req.get_msg_data<grand_tichu_req>().grand_tichu_call;

                    if (game_instance->call_grand_tichu(player, grand_tichu_call, err)) {
                        return {};
                    } else {
                        return ServerMsg(server_message{MessageType::Info, err});
                    }
                }
                auto resp = server_message{MessageType::Error, err };
                return ServerMsg(resp);
            }

                // ##################### SMALL TICHU ##################### //
            case ClientMsgType::call_small_tichu: {
                auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
                if (game_and_player) {
                    auto [player, game_instance] = game_and_player.value();
                    auto small_tichu_call = req.get_msg_data<small_tichu_req>().small_tichu_call;

                    if (game_instance->call_small_tichu(player, small_tichu_call, err)) {
                        return {};
                    } else {
                        return ServerMsg(server_message{MessageType::Info, err});
                    }
                }
                auto resp = server_message{MessageType::Error, err };
                return ServerMsg(resp);
            }

                // ##################### PLAY CARD ##################### //
            case ClientMsgType::play_combi: {
                auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
                if (game_and_player) {
                    auto [player, game_instance] = game_and_player.value();
                    auto combi = req.get_msg_data<play_combi_req>().played_combi;

                    if (game_instance->play_combi(player, combi, err)) {
                        return ServerMsg(full_state_response {game_instance->get_game_state()});
                    } else {
                        return ServerMsg(server_message{MessageType::Info, err});
                    }
                }
                auto resp = server_message{MessageType::Error, err };
                return ServerMsg(resp);
            }

            case ClientMsgType::ping: {
                return ServerMsg(pong {});
            }

            default: {
                auto resp = server_message{MessageType::Warn, "Unknown ClientMsgType " + std::to_string((int)type)};
                return ServerMsg(resp);
            }
        }
    }
}

#endif //TICHU_REQUEST_HANDLER_CPP
