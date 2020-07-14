#include "view.hxx"

// Convenient type aliases:
using Dimensions = ge211::Dimensions;
using Position   = ge211::Position;
using Color      = ge211::Color;
using Sprite_set = ge211::Sprite_set;

ge211::Color const black_color = ge211::Color::black(),
        white_color = ge211::Color::white(),
        red_color   = ge211::Color::medium_red(),
        green_color = ge211::Color::medium_green(),
        dark_grey_color = ge211::Color::from_rgba(128, 128, 128),
        very_light_grey_color = ge211::Color::from_rgba(220, 220, 220),
        light_grey_color = ge211::Color::from_rgba(211, 211, 211);


View::View(Model const& model)
        : model_(model)
        , grid_offset_(compute_grid_offset(model_))
        , player_black_token_(7, black_color)
        , player_white_token_(7, white_color)
        , player_grey_token_(7, dark_grey_color)
        , position_sprite_(8, red_color)
        , background_black_sprite(this->initial_window_dimensions(), black_color)
        , background_grey_sprite(this->initial_window_dimensions(),
                light_grey_color)
        , black_turn_sprite(text_generate("BLACK TURN"))
        , white_turn_sprite(text_generate("WHITE TURN"))
        , black_wins_sprite(text_generate("BLACK WINS"))
        , white_wins_sprite(text_generate("WHITE WINS"))
        , neither_wins_sprite(text_generate("DRAW"))
        // You may want to add sprite initialization here
{ }

const int grid_size_ = 23;

void View::draw(Sprite_set& set)
{
    Position centre = model_.board().center();

    for(int i = 0; i < model_.board().dimensions().width; i++) {
        for (int j = 0; j < model_.board().dimensions().height; j++) {

            set.add_sprite(background_black_sprite, {0,0}, 0);
            Position on_board = {i, j};
            ge211::Position screen_pos = board_to_screen_(on_board);
            set.add_sprite(board_sprite_, screen_pos, 1);
            const Move* move = model_.find_move(on_board);

            if(model_.is_game_over()) {
                ge211::Rectangle board = model_.board();
                set.add_sprite(background_grey_sprite, {0,0}, 0);
                if(model_.winner() == Player::light) {
                    set.add_sprite(white_wins_sprite, {
                            (initial_window_dimensions()
                                     .width / 2 - 50), 0}, 4);
                    for (Position each_pos: board) {
                        ge211::Position final_screen_pos =
                                                board_to_screen_(each_pos);
                        if(model_.operator[](each_pos) == Player::dark){
                            set.add_sprite(player_grey_token_,
                                           final_screen_pos, 4);

                        }
                    }
                }
                else if(model_.winner() == Player::dark) {
                    set.add_sprite(black_wins_sprite, {
                            (initial_window_dimensions()
                                     .width / 2 - 50), 0}, 4);
                    for (Position each_pos: board) {
                        ge211::Position final_screen_pos =
                                                board_to_screen_(each_pos);
                        if(model_.operator[](each_pos) == Player::light){
                            set.add_sprite(player_grey_token_,
                                           final_screen_pos, 4);

                        }
                    }
                }
                else if (model_.winner() == Player::neither){
                    set.add_sprite(neither_wins_sprite, {
                            (initial_window_dimensions()
                                     .width / 2 - 50), 0}, 4);
                    for (Position each_pos: board) {
                        ge211::Position final_screen_pos =
                                                board_to_screen_(each_pos);
                        set.add_sprite(player_grey_token_,
                                       final_screen_pos, 4);
                    }
                }
            }
            else if(model_.turn() == Player::dark) {
                set.add_sprite(black_turn_sprite, {(initial_window_dimensions()
                .width / 2 - 50), 0}, 4);
            }
            else {
                set.add_sprite(white_turn_sprite, {(initial_window_dimensions()
                .width / 2 - 50), 0}, 4);
            }

            Player player = model_.operator[](on_board);
            if(player == Player::dark) {
                set.add_sprite(player_black_token_, screen_pos, 3);
            }
            else if(player == Player::light) {
                set.add_sprite(player_white_token_, screen_pos, 3);
            }

            if(move != nullptr && player == Player::neither) {
                set.add_sprite(position_sprite_, screen_pos, 2);
            }
        }
    }

}

Dimensions View::initial_window_dimensions() const
{
    // You can change this if you want:
    return 50 * model_.board().dimensions();
}

std::string View::initial_window_title() const
{
    // You can change this if you want:
    return "Reversi";
}

ge211::Position View::board_to_screen_(ge211::Position board_pos) const
{
    int x = grid_size_ * board_pos.x;
    int y = grid_size_ * (board_pos.y);
    return ge211::Position{x, y} + grid_offset_;
}

ge211::Position View::screen_to_board_(ge211::Position screen_pos) const
{
    screen_pos = screen_pos - grid_offset_;
    int col_no  = screen_pos.x / grid_size_;
    int row_no  = screen_pos.y / grid_size_;
    return {col_no, row_no};
}

ge211::Dimensions View::compute_grid_offset(Model const& model)
{
    ge211::Dimensions model_dims{model_.board().dimensions().width, model_
            .board().dimensions().height};
    return ( 50 * model_.board().dimensions() - grid_size_ * model_dims) / 2;
}

ge211::Text_sprite View::text_generate(std::string s)
{
    ge211::Text_sprite          sprite;
    ge211::Text_sprite::Builder string_build(sans30);
    string_build.color(white_color) << s;
    sprite.reconfigure(string_build);
    return sprite;
}