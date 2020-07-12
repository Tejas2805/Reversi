#pragma once

#include "model.hxx"
#include <ge211.hxx>

#include <string>

extern ge211::Color const green_color;

class View
{
public:
    explicit View(Model const&);

    // You will probably want to add arguments here so that the
    // controller can communicate UI state (such as a mouse or
    // cursor position):
    void draw(ge211::Sprite_set& set);

    ge211::Position board_to_screen_(ge211::Position board_pos) const;

    ge211::Position screen_to_board_(ge211::Position screen_pos) const;

    ge211::Dimensions initial_window_dimensions() const;

    std::string initial_window_title() const;

private:
    Model const& model_;
    ge211::Circle_sprite const player_white_token_;
    ge211::Circle_sprite const player_black_token_;
    ge211::Circle_sprite const player_grey_token_;
    ge211::Rectangle_sprite const background_black_sprite;
    ge211::Rectangle_sprite const background_white_sprite;
    ge211::Rectangle_sprite const background_grey_sprite;

    ge211::Rectangle_sprite const
                board_sprite_  {{16, 16}, green_color};

    ge211::Circle_sprite const position_sprite_;

    ge211::Dimensions grid_offset_;

    ge211::Dimensions compute_grid_offset(Model const& model);

};
