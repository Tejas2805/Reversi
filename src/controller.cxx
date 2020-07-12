#include "controller.hxx"

Controller::Controller(int size)
        : Controller(size, size)
{ }

Controller::Controller(int width, int height)
        : model_(width, height)
        , view_(model_)
{ }

void Controller::draw(ge211::Sprite_set& sprites)
{
    view_.draw(sprites);
}

void Controller::on_key(ge211::Key key)
{
    if (key.code() == 'q') quit();
}

void Controller::on_mouse_up(ge211::Mouse_button btn, ge211::Position position)
{
    if(model_.turn() == Player::neither) return;
    if(btn != ge211::Mouse_button::left) return;

    ge211::Position on_board = view_.screen_to_board_(position);
    if(model_.find_move(on_board)) {
        model_.play_move(on_board);
    }
}

ge211::Dimensions Controller::initial_window_dimensions() const
{
    return view_.initial_window_dimensions();
}

std::string Controller::initial_window_title() const
{
    return view_.initial_window_title();
}
