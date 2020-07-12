/*******************************************/
/*** DO NOT CHANGE ANYTHING IN THIS FILE ***/
/*******************************************/

#pragma once

#include <ge211.hxx>

#include <bitset>
#include <initializer_list>
#include <unordered_map>

// Forward declaration for a set of `ge211::Position`s. The actual
// definition is below.
class Position_set;

// A move. The two fields are:
//
//   - first:  the position played in
//   - second: all positions changed by the move, *including* `first`
using Move       = std::pair<ge211::Position const, Position_set>;

// A map from move positions to their change sets.  An
// `std::unordered_map<K, V>` acts like a container holding
// `std::pair<K, V>`s in which 1) the `K` values are distinct,
// and 2) pairs can be found easily by their `K` values. Thus
// a `Move_map` is a collection of `Move`s that we can look up
// by `first` (the position played in).
using Move_map   = std::unordered_map<ge211::Position, Position_set>;

// An efficient set of `ge211::Position`s.
//
// The allowed range of the positions is limited by the constant
// `coord_limit` below. Right now it's 8, which means that the largest
// position a set can hold is {7, 7}.
class Position_set
{
public:
    // The element type of the set.
    using value_type = ge211::Position;

    // All position coordinates must be between 0 (inclusive) and
    // `coord_limit` (exclusive).
    static constexpr int coord_limit = 8;

private:

    // Maximum number of possible positions.
    static constexpr size_t index_limit = coord_limit * coord_limit;

    // Private representation type.
    using bits_t = std::bitset<index_limit>;

public:

    ///
    /// CONSTRUCTORS
    ///

    // Constructs the default position set.
    Position_set() noexcept = default;

    // Constructs the position set containing the given positions.
    // For example:
    //
    //     return Position_set{p1, p2};
    //
    Position_set(std::initializer_list<value_type>);

    // Constructs a position set given an iterator range of positions.
    // For example:
    //
    //     std::vector<ge211::Position> v;
    //     ...
    //
    //     Position_set pos_set(v.begin(), v.end());
    //
    template <class FwdIter>
    explicit Position_set(FwdIter begin, FwdIter end);


    ///
    /// BASIC OPERATIONS
    ///

    // Returns whether this set is empty.
    bool empty() const;

    // Returns the number of positions in this set.
    size_t size() const;

    // Returns whether position `p` is present in this set.
    //
    // PRECONDITIONS:
    //  - 0 <= p.x < coord_limit
    //  - 0 <= p.y < coord_limit
    //
    bool operator[](value_type p) const;

    // A proxy class that allows assigning a `bool` to add or remove
    // a position. This is returned by `operator[](value_type)` to
    // allow assigning a bool to the result (see below).
    using reference = bits_t::reference;

    // Returns an object that, when assigned to, changes whether `p` is
    // in the set. For example:
    //
    //     set[pos] = false;
    //
    //
    // PRECONDITIONS:
    //  - 0 <= p.x < coord_limit
    //  - 0 <= p.y < coord_limit
    //
    reference operator[](value_type p);

    // Removes all positions from the set.
    void clear();


    ///
    /// SET-LIKE OPERATIONS
    ///

    // Sets the elements of `*this` to be the intersection `*this` and
    // `that`. That is, `*this` will contain only the positions that
    // were in both.
    Position_set& operator&=(Position_set that);

    // Sets the elements of `*this` to be the union `*this` and `that`.
    // That is, `*this` will contain all the positions of both.
    Position_set& operator|=(Position_set that);

    // Sets the elements of `*this` to be the symmetric
    // difference of `*this` and `that`. That is, `*this` will
    // contain the positions that were in one or the other but
    // not both.
    Position_set& operator^=(Position_set that);

    // Returns the intersection of two sets.
    Position_set operator&(Position_set) const;

    // Returns the union of two sets.
    Position_set operator|(Position_set) const;

    // Returns the symmetric difference of two sets.
    Position_set operator^(Position_set) const;

    // Returns the complement of this set, containing all positions
    // that `*this` does not, and no positions that it does.
    Position_set operator~() const;


    ///
    /// ITERATOR OPERATIONS
    ///

    // An iterator over the positions of a set of positions. This is
    // used not only for iteration, but as the result of `find(value_type)`,
    // which checks for set membership.
    class iterator;

    // Returns an iterator to the first position in the set.
    iterator begin() const;

    // Returns an iterator one past the end of the set.
    iterator end() const;


    ///
    /// FRIENDS
    ///

    // Are two position sets equal?
    friend bool operator==(Position_set, Position_set);

    // Are two position sets unequal?
    friend bool operator!=(Position_set, Position_set);

private:

    // Private helpers
    static size_t index_of_checked_(value_type);
    static size_t index_of_(value_type);
    static value_type position_of_(size_t);
    iterator iterator_(value_type) const;

    // Data
    bits_t bits_;
};

class Position_set::iterator
        : public std::iterator<
                std::input_iterator_tag,
                Position_set::value_type const>
{
public:
    // Returns the current `Position`.
    value_type operator*() const;

    // Returns a pointer to the current `Position`.
    value_type* operator->() const;

    // Advances to the next element of the set.
    iterator& operator++();

    // Retreats to the previous element of the set.
    iterator& operator--();

    // Advances to the next element of the set.
    iterator operator++(int);

    // Retreats to the previous element of the set.
    iterator operator--(int);

private:
    friend Position_set;

    iterator(value_type, bits_t const*) noexcept;

    bool stopping_point_(size_t index) const;

    bits_t const*             bits_;
    Position_set::value_type  current_;
};

// Two iterators are equal when they dereference to
// the same position (even if they refer to different
// `Position_set`s).
bool operator==(Position_set::iterator, Position_set::iterator);

// Two iterators are unequal when they aren't equal.
bool operator!=(Position_set::iterator, Position_set::iterator);

// Prints a `Position_set`; suitable for debugging.
std::ostream& operator<<(std::ostream&, Position_set);

// Prints a `Move`; suitable for debugging.
std::ostream& operator<<(std::ostream&, Move const&);

// Implementation of range constructor (declared in class above).
template <class FwdIter>
Position_set::Position_set(FwdIter begin, FwdIter end)
{
    for ( ; begin != end; ++begin)
        (*this)[*begin] = true;
}

