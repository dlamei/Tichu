#include "gtest/gtest.h"
#include "../src/common/network/client_msg.h"
#include "../src/common/network/server_msg.h"
#include "../src/common/game_state/game_state.h"
#include "../src/common/game_state/cards/draw_pile.h"
#include "../src/common/game_state/cards/discard_pile.h"


// here we test if the serialization into json and parsing from json works for all types of objects, requests and responses

// helper function for printing the json
void print_json(const rapidjson::Document &doc) {
    std::cout << "json: " << json_utils::to_string(doc) << std::endl;
}

TEST(SerializationTest, JoinGameRequest) {
    auto join_game = join_game_req {"name"};
    auto player_id = UUID::create();
    auto game_id = UUID::create();

    auto send = client_msg(player_id, game_id, join_game);
    auto receive = client_msg::from_json(*send.to_json());

    EXPECT_EQ(receive.get_type(), ClientMsgType::join_game);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    EXPECT_EQ(receive.get_msg_data<join_game_req>().player_name, join_game.player_name);
}

TEST(SerializationTest, StartGameRequest) {
    auto player_id = UUID::create();
    auto game_id = UUID::create();;

    auto send = client_msg(player_id, game_id, start_game_req {});
    auto receive = client_msg::from_json(*send.to_json());

    EXPECT_EQ(receive.get_type(), ClientMsgType::start_game);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    ASSERT_NO_THROW(receive.get_msg_data<start_game_req>());
}

TEST(SerializationTest, FoldRequest) {
    auto player_id = UUID::create();
    auto game_id = UUID::create();;

    auto send = client_msg(player_id, game_id, fold_req {});
    auto receive = client_msg::from_json(*send.to_json());

    EXPECT_EQ(receive.get_type(), ClientMsgType::fold);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    ASSERT_NO_THROW(receive.get_msg_data<fold_req>());
}

/*
TEST(SerializationTest, DrawCardRequest) {
    auto draw_card = draw_card_req { 314 };
    auto player_id = UUID::create();
    auto game_id = UUID::create();;

    auto send = client_msg(player_id, game_id, draw_card);
    auto receive = client_msg::from_json(*send.to_json());

    EXPECT_EQ(receive.get_type(), ClientMsgType::draw_card);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    EXPECT_EQ(receive.get_msg_data<draw_card_req>().nof_cards, draw_card.nof_cards);
}
*/

TEST(SerializationTest, Card) {
    auto send = card(1, 2, 3);
    auto json = send.to_json();
    auto receive = card::from_json(*json);

    EXPECT_EQ(send.get_rank(), receive.get_rank());
    EXPECT_EQ(send.get_suit(), receive.get_suit());
    EXPECT_EQ(send.get_suit(), receive.get_suit());
    EXPECT_EQ(send, receive);
}


TEST(SerializationTest, Hand) {
    std::vector<card> cards = {
            {card(11, 12, 13), card(14, 15, 16), card(17, 18, 19)},
    };
    auto send = hand(cards);
    auto json = send.to_json();
    auto receive = hand::from_json(*json).value();

    EXPECT_EQ(send.get_nof_cards(), receive.get_nof_cards());

    for (int i = 0; i < receive.get_nof_cards(); i++) {
        EXPECT_EQ(send.get_cards()[i], receive.get_cards()[i]);
    }

    EXPECT_EQ(send, receive);
}

TEST(SerializationTest, Player) {
    std::vector<card> cards = {
            {card(11, 12, 13), card(14, 15, 16), card(17, 18, 19)},
    };
    auto send = player(UUID::create(), "name", 42, hand(cards), false);
    auto json = send.to_json();
    auto receive = player::from_json(*json);

    EXPECT_EQ(send.get_nof_cards(), receive.get_nof_cards());
    EXPECT_EQ(send.get_id(), receive.get_id());
    EXPECT_EQ(send.get_player_name(), receive.get_player_name());
    EXPECT_EQ(send.get_game_id(), receive.get_game_id());
    EXPECT_EQ(send.get_score(), receive.get_score());
    EXPECT_EQ(send.get_hand(), receive.get_hand());

}

TEST(SerializationTest, DiscardPile) {
    std::vector<card> cards = {
            {card(11, 12, 13), card(14, 15, 16), card(17, 18, 19)},
    };
    auto send = discard_pile(cards);
    auto json = send.to_json();
    auto receive = discard_pile::from_json(*json);

    EXPECT_EQ(send.get_cards(), receive.get_cards());
}

TEST(SerializationTest, DrawPile) {
    std::vector<card> cards = {
            {card(11, 12, 13), card(14, 15, 16), card(17, 18, 19)},
    };
    auto send = draw_pile(cards);
    auto json = send.to_json();
    auto receive = draw_pile::from_json(*json);

    EXPECT_EQ(send.get_cards(), receive.get_cards());
}

TEST(SerializationTest, FullStateResponse) {
    std::vector<std::vector<card>> all_cards = {
            {card(11, 12, 13), card(14, 15, 16), card(17, 18, 19)},
            {card(21, 22, 23), card(24, 25, 26), card(27, 28, 29)},
            {card(31, 32, 33), card(34, 35, 36), card(37, 38, 39)},
            {card(41, 42, 43), card(44, 45, 46), card(47, 48, 49)},

            {card(51, 52, 53), card(54, 55, 56), card(57, 58, 59)},
            {card(61, 62, 63), card(64, 65, 66), card(67, 68, 69)},
    };

    std::vector<player> players = {
    player(UUID::create(), "player0", 1, hand(all_cards[0]), false),
    player(UUID::create(), "player1", 2, hand(all_cards[1]), true),
    player(UUID::create(), "player2", 3, hand(all_cards[2]), false),
    player(UUID::create(), "player3", 4, hand(all_cards[3]), true)};

    auto draw = draw_pile(all_cards[4]);
    auto discard = discard_pile(all_cards[5]);

    auto state = game_state(UUID::create(), draw, discard, players, true, false, 0, 0, 3, 1);
    auto send_json = state.to_json();

    print_json(*send_json);
    auto received = game_state::from_json(*send_json);

    EXPECT_EQ(received.get_id(), state.get_id());
}
