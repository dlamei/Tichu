#include "gtest/gtest.h"
#include "../src/common/Messages.h"
#include "../src/common/game_state/cards/CardCombination.h"

TEST(CombiTest, Singles){
    std::vector<Card> single_two;
    single_two.push_back(Card(TWO, RED, 0));
    CardCombination combi_single_two(single_two);

    std::vector<Card> single_king;
    single_king.push_back(Card(KING, BLUE, 10));
    CardCombination combi_single_king(single_king);

    std::vector<Card> dragon;
    dragon.push_back(DRAGON);
    CardCombination combi_dragon(dragon);

    EXPECT_EQ(combi_single_two.get_combination_type(), SINGLE);
    EXPECT_EQ(combi_single_two.get_combination_rank(), TWO);
    EXPECT_EQ(combi_single_king.get_combination_type(), SINGLE);
    EXPECT_EQ(combi_single_king.get_combination_rank(), KING);
    EXPECT_EQ(combi_dragon.get_combination_rank(), 15);
    EXPECT_EQ(combi_dragon.get_combination_rank(), 15);
}

TEST(CombiTest, Doubles){
    std::vector<Card> double_three;
    double_three.push_back(Card(THREE, SCHWARZ, 0));
    double_three.push_back(Card(THREE, GREEN , 0));
    CardCombination combi_double_three(double_three);

    std::vector<Card> double_ace;
    double_ace.push_back(Card(ACE, SCHWARZ, 0));
    double_ace.push_back(PHONIX);
    CardCombination combi_double_ace(double_ace);

    EXPECT_EQ(combi_double_three.get_combination_type(), DOUBLE);
    EXPECT_EQ(combi_double_three.get_combination_rank(), THREE);
    EXPECT_EQ(combi_double_ace.get_combination_type(), DOUBLE);
    EXPECT_EQ(combi_double_ace.get_combination_rank(), ACE);
}

TEST(CombiTest, Fullhouses){
    std::vector<Card> pure_fullhouse;
    pure_fullhouse.push_back(Card(FOUR, SCHWARZ, 0));
    pure_fullhouse.push_back(Card(FOUR, GREEN, 0));
    pure_fullhouse.push_back(Card(QUEEN, GREEN, 0));
    pure_fullhouse.push_back(Card(QUEEN, SCHWARZ, 0));
    pure_fullhouse.push_back(Card(QUEEN, RED, 0));
    CardCombination combi_pure_fullhouse(pure_fullhouse);

    std::vector<Card> phoenix_fullhouse1;
    phoenix_fullhouse1.push_back(Card(FOUR, SCHWARZ, 0));
    phoenix_fullhouse1.push_back(Card(FOUR, GREEN, 0));
    phoenix_fullhouse1.push_back(PHONIX);
    phoenix_fullhouse1.push_back(Card(QUEEN, SCHWARZ, 0));
    phoenix_fullhouse1.push_back(Card(QUEEN, RED, 0));
    CardCombination combi_phoenix_fullhouse1(phoenix_fullhouse1);

    std::vector<Card> phoenix_fullhouse2;
    phoenix_fullhouse2.push_back(Card(FOUR, SCHWARZ, 0));
    phoenix_fullhouse2.push_back(PHONIX);
    phoenix_fullhouse2.push_back(Card(TEN, GREEN, 0));
    phoenix_fullhouse2.push_back(Card(TEN, SCHWARZ, 0));
    phoenix_fullhouse2.push_back(Card(TEN, RED, 0));
    CardCombination combi_phoenix_fullhouse2(phoenix_fullhouse2);

    EXPECT_EQ(combi_pure_fullhouse.get_combination_type(), FULLHOUSE);
    EXPECT_EQ(combi_pure_fullhouse.get_combination_rank(), QUEEN);
    EXPECT_EQ(combi_phoenix_fullhouse1.get_combination_type(), FULLHOUSE);
    EXPECT_EQ(combi_phoenix_fullhouse1.get_combination_rank(), QUEEN);
    EXPECT_EQ(combi_phoenix_fullhouse2.get_combination_type(), FULLHOUSE);
    EXPECT_EQ(combi_phoenix_fullhouse2.get_combination_rank(), TEN);
}

TEST(CombiTest, Streets){
    std::vector<Card> pure_five_street;
    pure_five_street.push_back(Card(TWO, RED, 0));
    pure_five_street.push_back(Card(THREE, RED, 0));
    pure_five_street.push_back(Card(FOUR, GREEN, 0));
    pure_five_street.push_back(Card(FIVE, RED, 0));
    pure_five_street.push_back(Card(SIX, RED, 0));
    CardCombination combi_pure_five_street(pure_five_street);

    std::vector<Card> phoenix_five_street;
    phoenix_five_street.push_back(Card(THREE, RED, 0));
    phoenix_five_street.push_back(Card(FOUR, RED, 0));
    phoenix_five_street.push_back(PHONIX);
    phoenix_five_street.push_back(Card(SIX, RED, 0));
    phoenix_five_street.push_back(Card(SEVEN, RED, 0));
    CardCombination combi_phoenix_five_street(phoenix_five_street);

    EXPECT_EQ(combi_pure_five_street.get_combination_type(), STRASS);
    EXPECT_EQ(combi_pure_five_street.get_combination_rank(), TWO);
    EXPECT_EQ(combi_phoenix_five_street.get_combination_type(), STRASS);
    EXPECT_EQ(combi_phoenix_five_street.get_combination_rank(), THREE);
}

TEST(CombiTest, Bombs){
    std::vector<Card> five_street_bomb;
    five_street_bomb.push_back(Card(TWO, RED, 0));
    five_street_bomb.push_back(Card(THREE, RED, 0));
    five_street_bomb.push_back(Card(FOUR, RED, 0));
    five_street_bomb.push_back(Card(FIVE, RED, 0));
    five_street_bomb.push_back(Card(SIX, RED, 0));
    CardCombination combi_five_street_bomb(five_street_bomb);

    std::vector<Card> bomb;
    bomb.push_back(Card(TWO, RED, 0));
    bomb.push_back(Card(TWO, SCHWARZ, 0));
    bomb.push_back(Card(TWO, GREEN, 0));
    bomb.push_back(Card(TWO, BLUE, 0));
    CardCombination combi_bomb(bomb);

    EXPECT_EQ(combi_five_street_bomb.get_combination_type(), BOMB);
    EXPECT_EQ(combi_five_street_bomb.get_combination_rank(), TWO);
    EXPECT_EQ(combi_bomb.get_combination_type(), BOMB);
    EXPECT_EQ(combi_bomb.get_combination_rank(), TWO);
}

TEST(CombiTest, Passing){
    std::vector<Card> pass;
    CardCombination combi_pass(pass);

    EXPECT_EQ(combi_pass.get_combination_type(), PASS);
    EXPECT_EQ(combi_pass.get_combination_rank(), 0);
}

TEST(CombiTest, Nothing){
    std::vector<Card> nothing;
    nothing.push_back(Card(TWO, RED, 0));
    nothing.push_back(Card(TWO, GREEN, 0));
    nothing.push_back(Card(FOUR, GREEN, 0));
    nothing.push_back(Card(SIX, RED, 0));
    nothing.push_back(PHONIX);
    CardCombination combi_nothing(nothing);

    EXPECT_EQ(combi_nothing.get_combination_type(), NONE);
    EXPECT_EQ(combi_nothing.get_combination_rank(), 0);
}

TEST(CombiTest, Switching){
    std::vector<Card> dog;
    dog.push_back(HUND);
    CardCombination combi_dog(dog);

    EXPECT_EQ(combi_dog.get_combination_type(), SWITCH);
    EXPECT_EQ(combi_dog.get_combination_rank(), 0);
}

TEST(CombiTest, CanBePlayedOn) {

    std::vector<Card> single_two;
    single_two.push_back(Card(TWO, RED, 0));
    CardCombination combi_single_two(single_two);

    std::vector<Card> single_king;
    single_king.push_back(Card(KING, BLUE, 10));
    CardCombination combi_single_king(single_king);

    std::vector<Card> phoenix;
    phoenix.push_back(PHONIX);
    CardCombination combi_phoenix(PHONIX);

    std::vector<Card> dragon;
    dragon.push_back(DRAGON);
    CardCombination combi_dragon(dragon);

    std::vector<Card> double_three;
    double_three.push_back(Card(THREE, SCHWARZ, 0));
    double_three.push_back(Card(THREE, GREEN , 0));
    CardCombination combi_double_three(double_three);

    std::vector<Card> double_ace;
    double_ace.push_back(Card(ACE, SCHWARZ, 0));
    double_ace.push_back(PHONIX);
    CardCombination combi_double_ace(double_ace);

    std::vector<Card> pure_fullhouse;
    pure_fullhouse.push_back(Card(FOUR, SCHWARZ, 0));
    pure_fullhouse.push_back(Card(FOUR, GREEN, 0));
    pure_fullhouse.push_back(Card(QUEEN, GREEN, 0));
    pure_fullhouse.push_back(Card(QUEEN, SCHWARZ, 0));
    pure_fullhouse.push_back(Card(QUEEN, RED, 0));
    CardCombination combi_pure_fullhouse(pure_fullhouse);
    
    std::vector<Card> phoenix_fullhouse1;
    phoenix_fullhouse1.push_back(Card(FOUR, SCHWARZ, 0));
    phoenix_fullhouse1.push_back(Card(FOUR, GREEN, 0));
    phoenix_fullhouse1.push_back(PHONIX);
    phoenix_fullhouse1.push_back(Card(QUEEN, SCHWARZ, 0));
    phoenix_fullhouse1.push_back(Card(QUEEN, RED, 0));
    CardCombination combi_phoenix_fullhouse1(phoenix_fullhouse1);
    
    std::vector<Card> phoenix_fullhouse2;
    phoenix_fullhouse2.push_back(Card(FOUR, SCHWARZ, 0));
    phoenix_fullhouse2.push_back(PHONIX);
    phoenix_fullhouse2.push_back(Card(TEN, GREEN, 0));
    phoenix_fullhouse2.push_back(Card(TEN, SCHWARZ, 0));
    phoenix_fullhouse2.push_back(Card(TEN, RED, 0));
    CardCombination combi_phoenix_fullhouse2(phoenix_fullhouse2);
    
    std::vector<Card> pure_five_street;
    pure_five_street.push_back(Card(TWO, RED, 0));
    pure_five_street.push_back(Card(THREE, RED, 0));
    pure_five_street.push_back(Card(FOUR, GREEN, 0));
    pure_five_street.push_back(Card(FIVE, RED, 0));
    pure_five_street.push_back(Card(SIX, RED, 0));
    CardCombination combi_pure_five_street(pure_five_street);
    
    std::vector<Card> phoenix_five_street;
    phoenix_five_street.push_back(Card(THREE, RED, 0));
    phoenix_five_street.push_back(Card(FOUR, RED, 0));
    phoenix_five_street.push_back(PHONIX);
    phoenix_five_street.push_back(Card(SIX, RED, 0));
    phoenix_five_street.push_back(Card(SEVEN, RED, 0));
    CardCombination combi_phoenix_five_street(phoenix_five_street);
    
    std::vector<Card> five_street_bomb;
    five_street_bomb.push_back(Card(TWO, RED, 0));
    five_street_bomb.push_back(Card(THREE, RED, 0));
    five_street_bomb.push_back(Card(FOUR, RED, 0));
    five_street_bomb.push_back(Card(FIVE, RED, 0));
    five_street_bomb.push_back(Card(SIX, RED, 0));
    CardCombination combi_five_street_bomb(five_street_bomb);
    
    std::vector<Card> bomb;
    bomb.push_back(Card(TWO, RED, 0));
    bomb.push_back(Card(TWO, SCHWARZ, 0));
    bomb.push_back(Card(TWO, GREEN, 0));
    bomb.push_back(Card(TWO, BLUE, 0));
    CardCombination combi_bomb(bomb);
    

    std::vector<Card> pass;
    CardCombination combi_pass(pass);

    std::vector<Card> nothing;
    nothing.push_back(Card(TWO, RED, 0));
    nothing.push_back(Card(TWO, GREEN, 0));
    nothing.push_back(Card(FOUR, GREEN, 0));
    nothing.push_back(PHONIX);
    nothing.push_back(Card(SIX, RED, 0));
    CardCombination combi_nothing(nothing);
    
    std::vector<Card> dog;
    dog.push_back(HUND);
    CardCombination combi_dog(dog);
    
    std::string err;
    // SINGLE
    EXPECT_EQ(combi_single_king.can_be_played_on(combi_single_two, err), true);
    EXPECT_EQ(combi_single_two.can_be_played_on(combi_single_king, err), false);

    // DRAGON & PHOENIX
    EXPECT_EQ(combi_phoenix.can_be_played_on(combi_single_two, err), true);
    EXPECT_EQ(combi_phoenix.get_combination_rank(), 2);
    EXPECT_EQ(combi_phoenix.can_be_played_on(combi_single_king, err), true);
    EXPECT_EQ(combi_phoenix.get_combination_rank(), 13);
    EXPECT_EQ(combi_phoenix.can_be_played_on(combi_dragon, err), false);
    
    // DOUBLE
    EXPECT_EQ(combi_double_ace.can_be_played_on(combi_double_three, err), true);
    EXPECT_EQ(combi_double_three.can_be_played_on(combi_double_ace, err), false);
    
    // WRONG COMBI
    EXPECT_EQ(combi_single_king.can_be_played_on(combi_double_ace, err), false);
    EXPECT_EQ(combi_double_ace.can_be_played_on(combi_single_two, err), false);
    
    // FULLHOUSE
    EXPECT_EQ(combi_pure_fullhouse.can_be_played_on(combi_phoenix_fullhouse1, err), false);
    EXPECT_EQ(combi_pure_fullhouse.can_be_played_on(combi_phoenix_fullhouse2, err), true);

    // STREET
    EXPECT_EQ(combi_phoenix_five_street.can_be_played_on(combi_pure_five_street, err), true);
    EXPECT_EQ(combi_pure_five_street.can_be_played_on(combi_phoenix_five_street, err), false);

    // BOMBS
    EXPECT_EQ(combi_bomb.can_be_played_on(combi_single_two, err), true);
    EXPECT_EQ(combi_bomb.can_be_played_on(combi_double_ace, err), true);
    EXPECT_EQ(combi_bomb.can_be_played_on(combi_pure_five_street, err), true);

    EXPECT_EQ(combi_five_street_bomb.can_be_played_on(combi_single_two, err), true);
    EXPECT_EQ(combi_five_street_bomb.can_be_played_on(combi_double_ace, err), true);
    EXPECT_EQ(combi_five_street_bomb.can_be_played_on(combi_pure_five_street, err), true);

    EXPECT_EQ(combi_five_street_bomb.can_be_played_on(combi_bomb, err), true);
    EXPECT_EQ(combi_bomb.can_be_played_on(combi_five_street_bomb, err), false);


    // NOTHING
    EXPECT_EQ(combi_nothing.can_be_played_on(combi_single_two, err), false);
    EXPECT_EQ(combi_nothing.can_be_played_on({}, err), false);
    
    // PASS
    EXPECT_EQ(combi_pass.can_be_played_on(combi_single_king, err), true);
    EXPECT_EQ(combi_pass.can_be_played_on(combi_pure_five_street, err), true);
    EXPECT_EQ(combi_pass.can_be_played_on(combi_bomb, err), true);

    // DOG
    EXPECT_EQ(combi_dog.can_be_played_on(combi_single_king, err), false);
    EXPECT_EQ(combi_dog.can_be_played_on({}, err), true);
    EXPECT_EQ(combi_single_king.can_be_played_on(combi_dog, err), true);
    EXPECT_EQ(combi_pure_five_street.can_be_played_on(combi_dog, err), true);
    EXPECT_EQ(combi_double_three.can_be_played_on(combi_dog, err), true);

} 