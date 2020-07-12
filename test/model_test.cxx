#include "model.hxx"
#include <catch.hxx>

using namespace ge211;

struct Test_access
{
    Model& m_ ;
    Board& board()
    {
        return m_.board_ ;
    }
    Position_set find_flips( Position start, Dimensions dir )
    {
        return m_.find_flips_(start, dir);
    }
    void set_game_over()
    {
        return m_.set_game_over_();
    }
    void really_play_move(Move move)
    {
        return m_.really_play_move_(move);
    }
    bool advance_turn()
    {
        return m_.advance_turn_();
    }
    Move_map compute_next_moves()
    {
        m_.compute_next_moves_();
        return m_.next_moves_;
    }
    Position_set evaluate_position(Position position)
    {
        return m_.evaluate_position_(position);
    }
};

// These pass with the starter code and should continue
// to pass.
TEST_CASE("Passes with starter code")
{
    Model m(8, 8);
    CHECK( m.board() == Rectangle{0, 0, 8, 8} );
    CHECK_FALSE( m.is_game_over() );
    CHECK( m.turn() == Player::dark );
    CHECK( m.winner() == Player::neither );
    CHECK( m[{0, 0}] == Player::neither );
    CHECK( m.find_move({0, 0}) == nullptr );
    CHECK_THROWS_AS( m.play_move({0, 0}), Client_logic_error );
}

// This fails with the starter code, but should pass.
TEST_CASE("Find move")
{
    Model m(6);

    CHECK( m.find_move({2, 2}) );
}

// This fails with the starter code, but should pass.
TEST_CASE("Play move")
{
    Model m(4);
    m.play_move({1, 1});
}

TEST_CASE("simple flips")
{
    Model model;
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    Position_set f = {};
    CHECK(f.empty());
    f = t.find_flips({2,4}, {0, 1});
    CHECK(f.empty());
    f = t.find_flips({2,4}, {0, -1});
    CHECK(f == Position_set{{2, 3}});
}

TEST_CASE("check game over with clear winner")
{
    Model model;
    Test_access t{model};
    CHECK(t.m_.winner() == Player :: neither);
    t.board()[{2 , 2}] = Player :: dark ;
    t.board()[{2 , 3}] = Player :: dark ;
    CHECK(t.m_.winner() == Player :: neither);
    t.set_game_over();
    CHECK(t.m_.winner() == Player :: dark);
}

TEST_CASE("check game over with equal")
{
    Model model;
    Test_access t{model};
    t.board()[{2 , 2}] = Player :: dark ;
    t.board()[{2 , 3}] = Player :: light ;
    CHECK(t.m_.winner() == Player :: neither);
    t.set_game_over();
    CHECK(t.m_.winner() == Player :: neither);
}

TEST_CASE("Simple advance_turn")
{
    Model model;
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    Position position = {2, 4};
    Position_set positionSet = {{2,3}};
    Move move = {position, positionSet};
    CHECK(t.m_.turn() == Player::dark);
    CHECK(t.advance_turn());
}

TEST_CASE("complicated advance_turn")
{
    Model model(6);
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::dark;
    t.board()[{3,2}] = Player::dark;
    t.board()[{3,3}] = Player::dark;

    Position position = {2, 4};
    Position_set positionSet = {{2,3}};
    Move move = {position, positionSet};
    CHECK(t.m_.turn() == Player::dark);
    CHECK_FALSE(t.advance_turn());
}


TEST_CASE("checks if a simple really_play_move works")
{
    Model model;
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    Position position = {2,4};
    Position_set position_set = {{2,3}};
    Move move = {position, position_set};
    CHECK(t.m_.turn() == Player::dark);
    t.really_play_move(move);
    CHECK(t.m_.operator[]({2,3}) == Player::dark);
    CHECK(t.m_.operator[]({2,4}) == Player::dark);
}

TEST_CASE("checks if a simple compute_move works")
{
    Model model(6);
    Test_access t{model};
    Move_map moves = t.compute_next_moves();
    CHECK_FALSE(moves.empty());
    CHECK(moves.size() == 4 );
}

TEST_CASE("checks if a complicated compute_move works")
{
    Model model(6);
    Test_access t{model};
    Position position = {2,2};
    Position_set position_set = {{2,2}};
    Move move = {position, position_set};
    CHECK(t.m_.turn() == Player::dark);
    model.play_move(position);
   // t.really_play_move(move);

    position = {2,3};
    position_set = {{2,3}};
    Move move1 = {position, position_set};
    CHECK(t.m_.turn() == Player::light);
    t.really_play_move(move1);

    position = {3,2};
    position_set = {{3,2}};
    Move move2 = {position, position_set};
    CHECK(t.m_.turn() == Player::dark);
    t.really_play_move(move2);

    position = {3,3};
    position_set = {{3,3}};
    Move move3 = {position, position_set};
    CHECK(t.m_.turn() == Player::light);
    t.really_play_move(move3);

    Move_map moves = t.compute_next_moves();
    CHECK_FALSE(moves.empty());
    CHECK(moves.size() == 4 );
}

TEST_CASE("Play move and then compute moves")
{
    Model m(4);
    m.play_move({1, 1});
    Test_access t{m};
    Move_map moves = t.compute_next_moves();
    CHECK_FALSE(moves.empty());
    CHECK(moves.size() == 3 );
}

TEST_CASE("Flip more than one in different directions")
{
    Model model(6);
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    t.board()[{4,2}] = Player::dark;
    t.board()[{3,3}] = Player::light;
    Position_set f = {};
    CHECK(f.empty());
    f = t.evaluate_position({2,4});
    CHECK_FALSE(f.empty());
    t.really_play_move({{2,4}, f});
    CHECK(t.m_.operator[]({2,3}) == Player::dark);
    CHECK(t.m_.operator[]({2,4}) == Player::dark);
    CHECK(t.m_.operator[]({3,3}) == Player::dark);
}


TEST_CASE("Flip more than one in same direction")
{
    Model model(6);
    Test_access t{model};
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    t.board()[{2,4}] = Player::light;
    Position_set f = {};
    CHECK(f.empty());
    f = t.evaluate_position({2,5});
    CHECK_FALSE(f.empty());
    t.really_play_move({{2,5}, f});
    CHECK(t.m_.operator[]({2,3}) == Player::dark);
    CHECK(t.m_.operator[]({2,4}) == Player::dark);
    CHECK(t.m_.operator[]({2,5}) == Player::dark);
}

TEST_CASE("Player repeat")
{
    Model model(4);
    Test_access t{model};
    t.board()[{0,1}] = Player::dark;
    t.board()[{0,3}] = Player::dark;
    t.board()[{1,1}] = Player::dark;
    t.board()[{1,3}] = Player::dark;
    t.board()[{2,2}] = Player::dark;
    t.board()[{2,3}] = Player::dark;
    t.board()[{2,1}] = Player::light;
    t.board()[{1,2}] = Player::light;
    Position_set f = {};
    CHECK(f.empty());
    f = t.evaluate_position({0,2});
    CHECK_FALSE(f.empty());
    CHECK(t.m_.turn() == Player::dark);
    t.really_play_move({{0,2}, f});
    CHECK(t.m_.operator[]({1,2}) == Player::dark);
    CHECK(t.m_.turn() == Player::dark);

}

TEST_CASE("when 1 of 4 is empty")
{
    Model model(6);
    Test_access t{model};
    t.board()[{3,2}] = Player::dark;
    t.board()[{2,3}] = Player::dark;
    t.board()[{3,3}] = Player::light;
    t.board()[{3,1}] = Player::light;
    CHECK(model.turn() == Player::dark);
    CHECK(t.m_.operator[]({2,2}) == Player::neither);
    model.play_move({2,2});
    CHECK(t.m_.operator[]({2,2}) == Player::dark);
    CHECK(model.turn() == Player::light);
    Move_map moves = t.compute_next_moves();
    CHECK_FALSE(moves.empty());
    CHECK(moves.size() == 2 );
}

TEST_CASE("check simple evaluate position")
{
    Model model (6);
    Test_access t{model};

    t.board()[{3,3}] = Player::dark;
    t.board()[{2,3}] = Player::light;
    t.board()[{2,2}] = Player::dark;
    t.board()[{3,2}] = Player::light;

    Position_set ps = t.evaluate_position({5,5});
    CHECK(ps.empty());

    ps = t.evaluate_position({1,3});
    CHECK(ps == Position_set{{2,3}, {1,3}});
}

TEST_CASE("check by playing simple game")
{
    Model m(2);
    CHECK(m.board() == Rectangle{0, 0, 2, 2});
    CHECK_FALSE(m.is_game_over());
    CHECK(m.winner() == Player::neither);
    CHECK(m.turn() == Player::dark);
    m.play_move({0,0});
    CHECK(m.turn() == Player::light);
    m.play_move({0,1});
    CHECK(m.turn() == Player::dark);
    m.play_move({1,0});
    CHECK(m.turn() == Player::light);
    CHECK(m.winner() == Player::neither);
    CHECK_FALSE(m.is_game_over());
    CHECK(m.find_move({1,1}));
    m.play_move({1,1});
    CHECK(m.is_game_over());
    CHECK(m.winner() == Player::neither);
}