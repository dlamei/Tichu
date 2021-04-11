//
// Created by Manuel Nowack on 10.04.21.
//

#include "gtest/gtest.h"
#include "../src/common/game_state/cards/card.h"

TEST(Card, can_be_played_on) {
    card c(1);
    ASSERT_TRUE(c.can_be_played_on((&c)));
}