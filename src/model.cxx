#include "model.hxx"
#include "iostream"

using namespace ge211;

Model::Model(int size)
        : Model(size, size)
{ }


int count = 0;

Model::Model(int width, int height)
        : board_({width, height})
{
    this->compute_next_moves_();
}

Rectangle Model::board() const
{
    return board_.all_positions();
}

Player Model::operator[](Position pos) const
{
    return board_[pos];
}

Move const* Model::find_move(Position pos) const
{
    auto i = next_moves_.find(pos);

    if (i == next_moves_.end()) {
        return nullptr;
    } else
        return &*i;
}

void Model::play_move(Position pos)
{
    if (is_game_over())
        throw Client_logic_error("Model::play_move: game over");

    Move const* movep = find_move(pos);
    if (!movep)
        throw Client_logic_error("Model::play_move: no such move");

    really_play_move_(*movep);
}

//
// BELOW ARE *OPTIONAL* HELPER FUNCTIONS
//

Position_set Model::find_flips_(Position current, Dimensions dir) const
{
    int  addition = 1;
    bool isValid  = false;

    Position_set original_position_set = {};
    Position_set new_position_set;

    while (true) {
        Position toCheck = current + addition * dir;

        if (!this->board_.good_position(toCheck)) {
            break;
        }

        if (this->board_[toCheck] == this->turn_) {
            isValid = true;
            break;
        } else if (this->board_[toCheck] == Player::neither) {
            break;
        } else {
            new_position_set      = {toCheck};
            original_position_set = original_position_set | new_position_set;
        }

        addition = addition + 1;
    }
    if (original_position_set.size() > 0 && isValid) {
        return original_position_set;
    } else {
        return {};
    }
}

Position_set Model::evaluate_position_(Position pos) const
{
    Position_set position_set;
    Dimensions   dimensions;
    dimensions.width  = -1;
    dimensions.height = -1;
    position_set = position_set | find_flips_(pos, dimensions);
    for (int i = 0; i < 2; i++) {
        dimensions.height = dimensions.height + 1;
        position_set = position_set | find_flips_(pos, dimensions);
    }

    dimensions.height = -1;

    for (int i = 0; i < 2; i++) {
        dimensions.width = dimensions.width + 1;
        position_set = position_set | find_flips_(pos, dimensions);
    }

    dimensions.width  = 1;
    dimensions.height = 1;
    position_set = position_set | find_flips_(pos, dimensions);

    dimensions.width  = 0;
    dimensions.height = 1;
    position_set = position_set | find_flips_(pos, dimensions);

    dimensions.width  = 1;
    dimensions.height = 0;
    position_set = position_set | find_flips_(pos, dimensions);

    if(position_set.size() > 0) {
        Position_set positionSet = {pos};
        position_set = position_set | positionSet;
    }

    return position_set;
}

void Model::compute_next_moves_()
{

    Rectangle     centre = this->board_.center_positions();
    count = 0;
    for (Position each_pos: centre) {
        if (this->board_[each_pos] == Player::neither) {
            Position_set position_set = {{each_pos}};
            this->next_moves_[each_pos] = {each_pos};
            count = count + 1;
        }
    }
    if (count == 0) {
        for (int i = 0; i < this->board_.dimensions().height; i++) {
            for (int j = 0; j < this->board_.dimensions().width; j++) {
                Position position = {i, j};
                Player   player   = this->board_[position];
                if (player != Player::neither) {
                    continue;
                }
                Position_set position_set = evaluate_position_(position);
                if (!position_set.empty()) {
                    this->next_moves_[position] = {position_set};
                }
            }
        }
    }
}

bool Model::advance_turn_()
{
    this->turn_ = other_player(this->turn_);
    this->next_moves_.clear();
    this->compute_next_moves_();
    return !this->next_moves_.empty();
}

void Model::set_game_over_()
{
    this->turn_ = Player::neither;
    size_t light_count = this->board_.count_player(Player::light);
    size_t dark_count  = this->board_.count_player(Player::dark);
    if (light_count > dark_count) {
        this->winner_ = Player::light;
    } else if (dark_count > light_count) {
        this->winner_ = Player::dark;
    } else {
        this->winner_ = Player::neither;
    }
}

void Model::really_play_move_(Move move)
{
    Position     position     = move.first;
    Position_set position_set = move.second;
    this->board_[position] = this->turn_;
    for (auto const& item : position_set) {
        this->board_[item] = this->turn_;
    }
    bool advance = this->advance_turn_();
    if (!advance) {
        bool advance_same = this->advance_turn_();
        if (!advance_same) {
            this->set_game_over_();
        }
    }
}
