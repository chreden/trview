#include "GoTo.h"
#include <trview.ui/TextArea.h>
#include <trview.ui/GroupBox.h>
#include "../Resources/resource.h"

namespace trview
{
    const std::string GoTo::Names::group{ "group" };
    const std::string GoTo::Names::text_area{ "text_area" };

    GoTo::GoTo(ui::Control& parent, const ui::UiSource& ui_source)
    {
        using namespace ui;

        _window = parent.add_child(ui_source(IDR_UI_GO_TO));
        _group = _window->find<GroupBox>(Names::group);
        _text_area = _window->find<TextArea>(Names::text_area);

        _token_store += _text_area->on_escape += [&]() { toggle_visible(); };
        _token_store += _text_area->on_enter += [&](const std::wstring& text)
        {
            try
            {
                auto index = std::stoul(text);
                on_selected(index);
                toggle_visible();
            }
            catch (...)
            {
                // Couldn't convert the number.
            }
        };

        const auto update_position = [&](const Size& size)
        {
            _window->set_position(Point(size.width / 2.0f - _window->size().width / 2.0f, size.height / 2.0f - _window->size().height / 2.0f));
        };
        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());
    }

    bool GoTo::visible() const
    {
        return _window->visible();
    }

    void GoTo::toggle_visible()
    {
        _window->set_visible(!visible());
        if (visible())
        {
            _text_area->set_text(L"");
            _text_area->on_focus_requested();
        }
        else
        {
            _text_area->on_focus_clear_requested();
        }
    }

    std::wstring GoTo::name() const
    {
        return _name;
    }

    void GoTo::set_name(const std::wstring& name)
    {
        _name = name;
        _group->set_title(L"Go to " + name);
    }
}
