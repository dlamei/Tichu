#include "gtest/gtest.h"
#include "../src/common/Messages.h"
#include <nlohmann/json.hpp>


// here we test if the serialization into json and parsing from json works for all types of objects, requests and responses

TEST(SerializationTest, JoinGameRequest) {
    auto join_game = join_game_req {"name"};
    auto player_id = UUID::create();
    auto game_id = UUID::create();

    auto send = ClientMsg(player_id, game_id, join_game);
    json data;
    to_json(data, send);
    ClientMsg receive;
    from_json(data, receive);

    EXPECT_EQ(receive.get_type(), ClientMsgType::join_game);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    EXPECT_EQ(receive.get_msg_data<join_game_req>().player_name, join_game.player_name);
}

TEST(SerializationTest, StartGameRequest) {
    auto player_id = UUID::create();
    auto game_id = UUID::create();;

    auto send = ClientMsg(player_id, game_id, start_game_req {});
    json data;
    to_json(data, send);
    ClientMsg receive;
    from_json(data, receive);

    EXPECT_EQ(receive.get_type(), ClientMsgType::start_game);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    ASSERT_NO_THROW(receive.get_msg_data<start_game_req>());
}

TEST(SerializationTest, FoldRequest) {
    auto player_id = UUID::create();
    auto game_id = UUID::create();;

    auto send = ClientMsg(player_id, game_id, fold_req {});
    json data;
    to_json(data, send);
    ClientMsg receive;
    from_json(data, receive);

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

    auto send = ClientMsg(player_id, game_id, draw_card);
    auto receive = ClientMsg::from_json(*send.to_json());

    EXPECT_EQ(receive.get_type(), ClientMsgType::draw_card);
    EXPECT_EQ(receive.get_game_id(), send.get_game_id());
    EXPECT_EQ(receive.get_player_id(), send.get_player_id());
    EXPECT_EQ(receive.get_msg_data<draw_card_req>().nof_cards, draw_card.nof_cards);
}
*/

TEST(SerializationTest, Card) {
    auto send = Card(1, 2, 3);
    json data;
    to_json(data, send);
    Card receive;
    from_json(data, receive);

    EXPECT_EQ(send.get_rank(), receive.get_rank());
    EXPECT_EQ(send.get_suit(), receive.get_suit());
    EXPECT_EQ(send.get_suit(), receive.get_suit());
    EXPECT_EQ(send, receive);
}


TEST(SerializationTest, Hand) {
    std::vector<Card> cards = {
            {Card(11, 12, 13), Card(14, 15, 16), Card(17, 18, 19)},
    };
    auto send = hand(cards);
    json data;
    to_json(data, send);
    hand receive;
    from_json(data, receive);

    EXPECT_EQ(send.get_nof_cards(), receive.get_nof_cards());

    for (int i = 0; i < receive.get_nof_cards(); i++) {
        EXPECT_EQ(send.get_cards()[i], receive.get_cards()[i]);
    }

    EXPECT_EQ(send, receive);
}

TEST(SerializationTest, Player) {
    auto player = Player("name");
    std::string err;
    player.add_card_to_hand(Card(11, 12, 13), err);
    player.add_card_to_hand(Card(12, 13, 14), err);
    player.add_card_to_hand(Card(10, 2, 1), err);
    auto send = Player("name");
    json data;
    to_json(data, send);
    Player receive;
    from_json(data, receive);

    EXPECT_EQ(send.get_nof_cards(), receive.get_nof_cards());
    EXPECT_EQ(send.get_id(), receive.get_id());
    EXPECT_EQ(send.get_player_name(), receive.get_player_name());
    EXPECT_EQ(send.get_game_id(), receive.get_game_id());
    EXPECT_EQ(send.get_hand(), receive.get_hand());

}

TEST(SerializationTest, DiscardPile) {
    std::vector<Card> cards1 = {
            {Card(11, 12, 13), Card(14, 15, 16), Card(17, 18, 19)},
    };
    std::vector<Card> cards2 = {
            {Card(11, 12, 13), Card(14, 15, 16), Card(17, 18, 19)},
    };
    CardCombination combi1(cards1);
    CardCombination combi2(cards2);
    std::vector<CardCombination> combis;
    combis.push_back(combi1);
    combis.push_back(combi2);
    auto send = ActivePile(combis);
    json data;
    to_json(data, send);
    ActivePile receive;
    from_json(data, receive);
}

TEST(SerializationTest, DrawPile) {
    std::vector<Card> cards = {
            {Card(11, 12, 13), Card(14, 15, 16), Card(17, 18, 19)},
    };
    auto send = DrawPile(cards);
    json data;
    to_json(data, send);
    DrawPile receive;
    from_json(data, receive);

    EXPECT_EQ(send.get_cards(), receive.get_cards());
}

TEST(SerializationTest, FullStateResponse) {
    std::vector<std::vector<Card>> all_cards = {
            {Card(11, 12, 13), Card(14, 15, 16), Card(17, 18, 19)},
            {Card(21, 22, 23), Card(24, 25, 26), Card(27, 28, 29)},
            {Card(31, 32, 33), Card(34, 35, 36), Card(37, 38, 39)},
            {Card(41, 42, 43), Card(44, 45, 46), Card(47, 48, 49)},
            {Card(51, 52, 53), Card(54, 55, 56), Card(57, 58, 59)},
    };

    std::vector<player_ptr> players = {
    std::make_shared<Player>(Player("alice")),
    std::make_shared<Player>(Player("bob")),
    std::make_shared<Player>(Player("carl")),
    std::make_shared<Player>(Player("dan"))};
    std::string err;
    for (int i = 0; i < 4; i++) {
        players[i]->add_card_to_hand(all_cards.at(i).at(0), err);
        players[i]->add_card_to_hand(all_cards.at(i).at(1), err);
        players[i]->add_card_to_hand(all_cards.at(i).at(2), err);
    }

    auto draw = DrawPile(all_cards[4]);
    std::vector<Player> round_finish_order = std::vector<Player>();
    auto state = GameState();
    state.add_player(players[0], err);
    state.add_player(players[1], err);
    state.add_player(players[2], err);
    state.add_player(players[3], err);

    json data;
    to_json(data, state);
    GameState receive;
    from_json(data, receive);

    EXPECT_EQ(receive.get_id(), state.get_id());
    EXPECT_EQ(*receive.get_players().at(0), *state.get_players().at(0));
    EXPECT_EQ(*receive.get_players().at(1), *state.get_players().at(1));
    EXPECT_EQ(*receive.get_players().at(2), *state.get_players().at(2));
    EXPECT_EQ(*receive.get_players().at(3), *state.get_players().at(3));
}
