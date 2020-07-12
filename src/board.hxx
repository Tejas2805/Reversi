/*******************************************/
/*** DO NOT CHANGE ANYTHING IN THIS FILE ***/
/*******************************************/

#pragma once

#include "player.hxx"
#include "move.hxx"

#include <ge211.hxx>

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

// Represents the state of the board.
class Board
{
public:
    // Constructs a board with the given dimensions.
    //
    // ERRORS:
    //
    //  - Throws `ge211::Client_logic_error` if either dimension is less
    //    than 2 or greater than 8.
    explicit Board(ge211::Dimensions dims);

    // Returns the same `ge211::Dimensions` value passed to the
    // constructor.
    ge211::Dimensions dimensions() const;

    // Returns whether the given position is in bounds.
    bool good_position(ge211::Position) const;

    // Returns the `Player` stored at `pos`.
    //
    // ERRORS:
    //  - throws `ge211::Client_logic_error` if `!good_position(pos)`.
    Player operator[](ge211::Position pos) const;

    // Defined and documented below.
    class reference;

    // Returns a reference to the `Player` stored at `pos`. This can
    // be assigned to update the board:
    //
    //     // Light player plays at (3, 4)
    //     board[{3, 4}] = Player::light;
    //
    // ERRORS:
    //  - throws `ge211::Client_logic_error` if `!good_position(pos)`.
    reference operator[](ge211::Position pos);

    // Stores the given player in all the positions in the given set.
    // For example,
    //
    //     // Sets three positions to dark:
    //     Position_set positions{{0, 0}, {1, 1}, {2, 2}};
    //     board.set_all(positions, Player::dark);
    //
    // ERRORS:
    //  - behavior is undefined if any positions in the `Position_set`
    //    are out of bounds.
    void set_all(Position_set, Player);

    // Counts the number of occurrences of the given player in the board.
    size_t count_player(Player) const;

    // Returns a rectangle containing all the positions of the board. This
    // can be used to iterate over the positions:
    //
    //     for (ge211::Position pos : a_board.all_positions()) {
    //         ... a_board[pos] ...;
    //     }
    //
    // Note that `ge211::Rectangles` are considered to be closed on the top
    // and left, but open on the bottom and right. The iterator will visit
    // the correct positions for the board.
    ge211::Rectangle all_positions() const;

    // Returns a rectangle containing the four center positions which
    // much be occupied for play to move beyond them. This can be used
    // to iterate over those positions.
    ge211::Rectangle center_positions() const;

    // Returns a reference to a `std::vector` containimg all eight "unit
    // direction vectors". In Python notation, these are:
    //
    //     { Dimensions(dx, dy)
    //           for dx in [-1, 0, 1]
    //               for dy in [-1, 0, 1]
    //                   if dx or dy }
    //
    static std::vector<ge211::Dimensions> const& all_directions();

    // Equality for boards.
    friend bool operator==(Board const&, Board const&);

    // Defined and documented below.
    class multi_reference;

    // Returns an object that allows assigning to all the positions in
    // `pset`. See below for the documentation of
    // `Board::multi_reference::operator=(Player)`.
    //
    // ERRORS:
    //  - behavior is undefined if any positions in the `Position_set`
    //    are out of bounds.
    multi_reference at_set(Position_set pset);

#ifdef BOARD_INDEX_OVERLOAD
    // Optional alias for `at_set`, so you can write
    //
    //     board[pset] = player;
    //
    // This is disabled by default because GCC can't distinguish
    // this overload from `operator[](Position)` when given something
    // like `{3, 4}`.
    multi_reference operator[](Position_set pset);
#endif // BOARD_INDEX_OVERLOAD


private:
    Player get_(ge211::Position) const;
    void set_(ge211::Position, Player);

    void bounds_check_(ge211::Position) const;

    ge211::Dimensions    dims_;
    Position_set         light_;
    Position_set         dark_;
    // Invariant: (light_ & dark_).empty()
};

// Inequality for boards.
bool operator!=(Board const&, Board const&);

// Board printing, suitable for debugging.
std::ostream& operator<<(std::ostream&, Board const&);

///
/// Helper classes
///

// Class returned by `operator[](ge211::Position)` that simulates
// an assignable reference to a `Position`. This is what allows
// you to write
//
//     board[pos] = player;
//
// to place `player` at `pos`.
//
// The definition of the class follows this definition of the
// `Board` class.
class Board::reference
{
    friend class Board;

public:
    // Assigns the value of `that` to the object of `this`.
    reference& operator=(reference const&) noexcept;

    // Assigns to the object of the reference.
    reference& operator=(Player) noexcept;

    // Returns the value of the reference.
    operator Player() const noexcept;

private:
    reference(Board&, ge211::Position) noexcept;

    Board&           board_;
    ge211::Position  pos_;
};


// Class returned by `at_set(Position_set)` that allows assigning
// one player to all the positions in the given `Position_set`.
class Board::multi_reference
{
    friend class Board;

public:
    // Assigns the given player to all the positions of this
    // multi-reference, which are all the positions in the set that
    // was given to `Board::at_set(Position_set)`. Thus, you can
    // mass-assign a player to a set of positions like so:
    //
    //     // Sets three positions to dark:
    //     Position_set positions{{0, 0}, {1, 1}, {2, 2}};
    //     board.at_set(positions) = Player::dark;
    //
    multi_reference& operator=(Player) noexcept;

private:
    multi_reference(Board&, Position_set) noexcept;

    Board&           board_;
    Position_set     pos_set_;
};

#ifdef BOARD_INDEX_OVERLOAD
inline Board::multi_reference Board::operator[](Position_set pset)
{
    return at_set(pset);
}
#endif // BOARD_INDEX_OVERLOAD

