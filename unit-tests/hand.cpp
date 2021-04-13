//
// Created by Manuel Nowack on 11.04.21.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/player/hand.h"
#include "../src/common/serialization/json_utils.h"

class HandTest : public ::testing::Test {

protected:
    virtual void SetUp() {
        cards.resize(8);
        for (int i = 1; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                cards[i].push_back(new card(i));
            }
        }
    }

    std::vector<std::vector<card*>> cards;
    hand player_hand;
    std::string err;
};

TEST_F(HandTest, AddOneCard) {
    player_hand.add_card(cards[1][0], err);
    std::vector<card*> expected = {cards[1][0]};
    ASSERT_EQ(expected, player_hand.get_cards());
}

TEST_F(HandTest, AddNoCards) {
    std::vector<card*> expected;
    ASSERT_EQ(expected, player_hand.get_cards());
}

TEST_F(HandTest, AddManyCards) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[7][0], err);
    std::vector<card*> expected = {cards[1][0], cards[3][0], cards[7][0]};
    ASSERT_EQ(expected, player_hand.get_cards());
}

TEST_F(HandTest, AddManyCardsWithDuplicates) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[1][1], err);
    player_hand.add_card(cards[1][2], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[3][1], err);
    player_hand.add_card(cards[7][0], err);
    std::vector<card*> expected = {cards[1][0], cards[1][1], cards[1][2],
                                   cards[3][0], cards[3][1], cards[7][0]};
    ASSERT_EQ(expected, player_hand.get_cards());
}

TEST_F(HandTest, ScoreOne) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
}

TEST_F(HandTest, ScoreLama) {
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(10, player_hand.get_score());
}

TEST_F(HandTest, ScoreEmpty) {
    ASSERT_EQ(0, player_hand.get_score());
}

TEST_F(HandTest, ScoreUnique) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(14, player_hand.get_score());
}

TEST_F(HandTest, ScoreDuplicate) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[1][1], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[1][2], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[3][1], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(14, player_hand.get_score());
}

TEST_F(HandTest, CountOne) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountEmpty) {
    ASSERT_EQ(0, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountMany) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(2, player_hand.get_nof_cards());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(3, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountManyWithDuplicates) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
    player_hand.add_card(cards[1][1], err);
    ASSERT_EQ(2, player_hand.get_nof_cards());
    player_hand.add_card(cards[1][2], err);
    ASSERT_EQ(3, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][1], err);
    ASSERT_EQ(5, player_hand.get_nof_cards());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(6, player_hand.get_nof_cards());
}
