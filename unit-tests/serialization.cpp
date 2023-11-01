#include "gtest/gtest.h"
#include "../../src/common/network/requests/client_request.h"

TEST(SerializationTest, JoinGameRequest) {
    auto join_game = client_request(UUID("player_id"), UUID("game_id"), join_game_request("player_name"));
    auto send = json_utils::to_string(*join_game.to_json());

    rapidjson::Document req_json;
    req_json.Parse(send);
    auto req = client_request::from_json(req_json);
    EXPECT_EQ(req.get_type(), RequestType::join_game);
}