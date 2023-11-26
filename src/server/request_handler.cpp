
#ifndef TICHU_REQUEST_HANDLER_CPP
#define TICHU_REQUEST_HANDLER_CPP

#include "request_handler.h"

#include "player_manager.h"
#include "game_instance_manager.h"
#include "game_instance.h"

#include "../common/network/client_msg.h"


server_msg request_handler::handle_request(const client_msg &req) {

    // Prepare variables that are used by every request type
    //player* player;
    std::string err;
    //game_instance* game_instance_ptr = nullptr;

    // Get common properties of requests
    ClientMsgType type = req.get_type();
    const auto &game_id = req.get_game_id();
    const auto &player_id = req.get_player_id();

    // Switch behavior according to request type
    switch (type) {

        // ##################### JOIN GAME #####################  //
        case ClientMsgType::join_game: {
            //std::string player_name = ((join_game_req *) req)->get_player_name();
            std::string player_name = req.get_msg_data<join_game_req>().player_name;

            // Create new player or get existing one with that name
            auto player = player_manager::add_or_get_player(player_name, player_id);

            if (game_id.empty()) {
                // join any game
                auto game_instance_ptr = game_instance_manager::try_add_player_to_any_game(player, err);
                if (game_instance_ptr) {
                    // game_instance_ptr got updated to the joined game
                    // return response with full game_state attached
                    auto resp = request_response{true, game_instance_ptr.value()->get_game_state().to_json(), err};
                    return server_msg(game_instance_ptr.value()->get_id(), resp);
                    //return request_response(game_instance_ptr.value()->get_id(), req_id, true, game_instance_ptr.value()->get_game_state().to_json(), err);
                } else {
                    // failed to find game to join
                    auto resp = request_response{false, {}, err};
                    return server_msg(game_instance_ptr.value()->get_id(), resp);
                }
            } else {
                // join a specific game denoted by req->get_game_id()
                auto game_instance = game_instance_manager::try_get_game_instance(game_id);
                if (game_instance) {
                    if (game_instance_manager::try_add_player(player, *game_instance.value(), err)) {
                        // return response with full game_state attached
                        auto resp = request_response{true,
                                                     game_instance.value()->get_game_state().to_json(), err};
                        return server_msg(game_id, resp);

                    } else {
                        // failed to join requested game
                        auto resp = request_response{false, {}, err};
                        return server_msg(UUID(), resp);
                    }
                } else {
                    // failed to find requested game
                    auto resp = request_response{false, {}, "Requested game could not be found."};
                    return server_msg(UUID(), resp);
                }
            }
        }


            // ##################### START GAME ##################### //
        case ClientMsgType::start_game: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                if (game_instance->start_game(player, err)) {
                    auto resp = request_response{true, game_instance->get_game_state().to_json(), err};
                    return server_msg(game_instance->get_id(), resp);
                }
            }
            auto resp = request_response{false, {}, err};
            return server_msg(UUID(), resp);
        }


            // ##################### PLAY CARD ##################### //
        case ClientMsgType::play_combi: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                auto combi = req.get_msg_data<play_combi_req>().played_combi;

                if (game_instance->play_combi(player, combi, err)) {
                    auto resp = request_response{true, game_instance->get_game_state().to_json(), err};
                    return server_msg(game_instance->get_id(), resp);
                }
            }
            auto [player, game_instance] = game_and_player.value();
            auto resp = request_response{false, game_instance->get_game_state().to_json(), err};
            return server_msg(game_instance->get_id(), resp);
        }

            // ##################### FOLD ##################### //
            /*
            case ClientMsgType::fold: {
                auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
                if (game_and_player) {
                    auto [player, game_instance] = game_and_player.value();
                    if (game_instance->fold(*player, err)) {
                        auto resp = request_response{true, game_instance->get_game_state().to_json(), err};
                        return server_msg(game_instance->get_id(), resp);
                    }
                }
                auto resp = request_response{false, {}, err};
                return server_msg(UUID(), resp);
            }
            */

            // ##################### UNKNOWN REQUEST ##################### //
        default: {
            auto resp = request_response{false, {}, "Unknown ClientMsgType " + std::to_string(type)};
            return server_msg(UUID(), resp);
        }
    }
}

#endif //TICHU_REQUEST_HANDLER_CPP
