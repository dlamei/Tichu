//
// Created by Manuel Nowack on 11.04.21.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/player/hand.h"

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
    ASSERT_TRUE(player_hand.add_card(cards[1][0], err));
    std::vector<card*> expected_hand = {cards[1][0]};
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, AddNoCards) {
    std::vector<card*> expected_hand = {};
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, AddManyCards) {
    ASSERT_TRUE(player_hand.add_card(cards[1][0], err));
    ASSERT_TRUE(player_hand.add_card(cards[3][0], err));
    ASSERT_TRUE(player_hand.add_card(cards[7][0], err));
    std::vector<card*> expected_hand = {cards[1][0], cards[3][0], cards[7][0]};
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, AddManyCardsWithDuplicates) {
    ASSERT_TRUE(player_hand.add_card(cards[1][0], err));
    ASSERT_TRUE(player_hand.add_card(cards[1][1], err));
    ASSERT_TRUE(player_hand.add_card(cards[1][2], err));
    ASSERT_TRUE(player_hand.add_card(cards[3][0], err));
    ASSERT_TRUE(player_hand.add_card(cards[3][1], err));
    ASSERT_TRUE(player_hand.add_card(cards[7][0], err));
    std::vector<card*> expected_hand = {cards[1][0], cards[1][1], cards[1][2],
                                        cards[3][0], cards[3][1], cards[7][0]};
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, RemoveOneCard) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[3][0], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, RemoveNonexistentCards) {
    card* to_remove = cards[1][0];
    card* removed_card = cards[1][0];
    ASSERT_FALSE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {};
    EXPECT_EQ(nullptr, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());

    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[7][0], err);

    to_remove = cards[1][1];
    removed_card = cards[1][1];
    ASSERT_FALSE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][0], cards[3][0], cards[7][0]};
    EXPECT_EQ(nullptr, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}


TEST_F(HandTest, RemoveAllCards) {
    player_hand.add_card(cards[1][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, RemoveManyCards) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[1][1], err);
    player_hand.add_card(cards[1][2], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[3][1], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[1][0];
    card* removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[1][1], cards[1][2], cards[3][0],
                                        cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
    to_remove = cards[3][0];
    removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][1], cards[1][2], cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
    to_remove = cards[7][0];
    removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][1], cards[1][2], cards[3][1]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, RemoveManyDuplicateCards) {
    player_hand.add_card(cards[1][0], err);
    player_hand.add_card(cards[1][1], err);
    player_hand.add_card(cards[1][2], err);
    player_hand.add_card(cards[3][0], err);
    player_hand.add_card(cards[3][1], err);
    player_hand.add_card(cards[7][0], err);

    card* to_remove = cards[3][0];
    card* removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    std::vector<card*> expected_hand = {cards[1][0], cards[1][1], cards[1][2],
                                        cards[3][1], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
    to_remove = cards[3][1];
    removed_card = nullptr;
    ASSERT_TRUE(player_hand.remove_card(to_remove->get_id(), removed_card, err));
    expected_hand = {cards[1][0], cards[1][1], cards[1][2], cards[7][0]};
    EXPECT_EQ(to_remove, removed_card);
    ASSERT_EQ(expected_hand, player_hand.get_cards());
}

TEST_F(HandTest, ScoreOneCard) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
}

TEST_F(HandTest, ScoreLama) {
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(10, player_hand.get_score());
}

TEST_F(HandTest, ScoreNoCards) {
    ASSERT_EQ(0, player_hand.get_score());
}

TEST_F(HandTest, ScoreManyCards) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_score());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(4, player_hand.get_score());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(14, player_hand.get_score());
}

TEST_F(HandTest, ScoreManyCardsWithDuplicates) {
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

TEST_F(HandTest, CountOneCard) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountNoCards) {
    ASSERT_EQ(0, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountManyCards) {
    player_hand.add_card(cards[1][0], err);
    ASSERT_EQ(1, player_hand.get_nof_cards());
    player_hand.add_card(cards[3][0], err);
    ASSERT_EQ(2, player_hand.get_nof_cards());
    player_hand.add_card(cards[7][0], err);
    ASSERT_EQ(3, player_hand.get_nof_cards());
}

TEST_F(HandTest, CountManyCardsWithDuplicates) {
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
