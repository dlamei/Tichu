//
// Created by Manuel on 08.02.2022.
//

#ifndef TICHU_REQUEST_HANDLER_CPP
#define TICHU_REQUEST_HANDLER_CPP

#include "request_handler.h"

#include "player_manager.h"
#include "game_instance_manager.h"
#include "game_instance.h"

//#include "../common/network/requests/join_game_request.h"
//#include "../common/network/requests/draw_card_request.h"
//#include "../common/network/requests/play_card_request.h"
#include "../common/network/requests/client_request.h"


server_response request_handler::handle_request(const client_request &req) {

    // Prepare variables that are used by every request type
    //player* player;
    std::string err;
    //game_instance* game_instance_ptr = nullptr;

    // Get common properties of requests
    RequestType type = req.get_type();
    const auto& req_id = req.get_req_id();
    const auto& game_id = req.get_game_id();
    const auto& player_id = req.get_player_id();


    // Switch behavior according to request type
    switch(type) {

        // ##################### JOIN GAME #####################  //
        case RequestType::join_game: {
            //std::string player_name = ((join_game_request *) req)->get_player_name();
            std::string player_name = req.get_request<join_game_request>().get_player_name();

            // Create new player or get existing one with that name
            auto player = player_manager::add_or_get_player(player_name, player_id);

            if (game_id.empty()) {
                // join any game
                auto game_instance_ptr = game_instance_manager::try_add_player_to_any_game(*player, err);
                if (game_instance_ptr) {
                    // game_instance_ptr got updated to the joined game

                    // return response with full game_state attached
                    auto resp = request_response{ req_id, true, game_instance_ptr.value()->get_game_state().to_json(), err };
                    return server_response(game_instance_ptr.value()->get_id(), resp);
                    //return request_response(game_instance_ptr.value()->get_id(), req_id, true, game_instance_ptr.value()->get_game_state().to_json(), err);
                } else {
                    // failed to find game to join
                    auto resp = request_response{req_id, false, nullptr, err};
                    return server_response(game_instance_ptr.value()->get_id(), resp);
                }
            } else {
                // join a specific game denoted by req->get_game_id()
                auto game_instance = game_instance_manager::try_get_game_instance(game_id);
                if (game_instance) {
                    if (game_instance_manager::try_add_player(*player, *game_instance.value(), err)) {
                        // return response with full game_state attached
                        auto resp = request_response{req_id, true,
                                                    game_instance.value()->get_game_state().to_json(), err};
                        return server_response(game_id, resp);

                    } else {
                        // failed to join requested game
                        auto resp = request_response(req_id, false, nullptr, err);
                        return server_response(UUID(), resp);
                    }
                } else {
                    // failed to find requested game
                    auto resp = request_response(req_id, false, {}, "Requested game could not be found.");
                    return server_response(UUID(), resp);
                }
            }
        }


        // ##################### START GAME ##################### //
        case RequestType::start_game: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                if (game_instance->start_game(*player, err)) {
                    auto resp = request_response(req_id, true, game_instance->get_game_state().to_json(), err);
                    return server_response(game_instance->get_id(), resp);
                }
            }
            auto resp = request_response(req_id, false, {}, err);
            return server_response(UUID(), resp);
        }


        // ##################### PLAY CARD ##################### //
        case RequestType::play_card: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                auto card_id = req.get_request<play_card_request>().get_card_id();
                //std::string card_id = ((play_card_request *) req)->get_card_id();
                if (game_instance->play_card(*player, card_id, err)) {
                    auto resp = request_response(req_id, true, game_instance->get_game_state().to_json(), err);
                    return server_response(game_instance->get_id(), resp);
                }
            }
            auto resp = request_response(req_id, false, {}, err);
            return server_response(UUID(), resp);
        }


        // ##################### DRAW CARD ##################### //
        case RequestType:: draw_card: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                if (game_instance->draw_card(*player, err)) {
                    auto resp = request_response(req_id, true, game_instance->get_game_state().to_json(), err);
                    return server_response(game_instance->get_id(), resp);
                }
            }
            auto resp = request_response(req_id, false, {}, err);
            return server_response(UUID(), resp);
        }


        // ##################### FOLD ##################### //
        case RequestType::fold: {
            auto game_and_player = game_instance_manager::try_get_player_and_game_instance(player_id, err);
            if (game_and_player) {
                auto [player, game_instance] = game_and_player.value();
                if (game_instance->fold(*player, err)) {
                    auto resp = request_response(req_id, true, game_instance->get_game_state().to_json(), err);
                    return server_response(game_instance->get_id(), resp);
                }
            }
            auto resp = request_response(req_id, false, {}, err);
            return server_response(UUID(), resp);
        }


        // ##################### UNKNOWN REQUEST ##################### //
        default: {
            auto resp = request_response(req_id, false, {}, "Unknown RequestType " + type);
            return server_response(UUID(), resp);
        }
    }
}

#endif //TICHU_REQUEST_HANDLER_CPP
