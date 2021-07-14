#include "Bubble.h"

using namespace trview::ui;
using namespace trview::graphics;

namespace trview
{
    Bubble::Bubble(Control& control)
    {
        _label = control.add_child(std::make_unique<Label>(Size(40, 20), Colour::Black, L"Copied", 8, TextAlignment::Centre, ParagraphAlignment::Centre));
        _label->set_visible(false);
        _label->set_z(-1);

        _token_store += control.on_update += [&](float delta)
        {
            if (_label->visible())
            {
                auto bg = _label->background_colour();
                bg.a = std::max(bg.a - delta, 0.0f);
                auto fg = _label->text_colour();
                fg.a = std::max(fg.a - delta, 0.0f);

                _label->set_text_colour(fg);
                _label->set_background_colour(bg);

                if (bg.a == 0.0f || fg.a == 0.0f)
                {
                    _label->set_visible(false);
                }
            }
        };
    }

    void Bubble::show()
    {
        _label->set_background_colour(Colour::Black);
        _label->set_text_colour(Colour::White);
        _label->set_visible(true);
    }
}
