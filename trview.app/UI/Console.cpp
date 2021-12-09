#include "Console.h"
#include "../Resources/resource.h"

using namespace trview::ui;

namespace trview
{
    const std::string Console::Names::log{ "log" };
    const std::string Console::Names::input{ "input" };

    Console::Console(Control& parent, const ILoader& ui_source)
    {
        _window = parent.add_child(ui_source.load_from_resource(IDR_UI_CONSOLE));
        
        _log = _window->find<TextArea>(Names::log);
        _token_store += _log->on_tab += [&](auto) { _input->on_focus_requested(); };

        _input = _window->find<TextArea>(Names::input);
        _token_store += _input->on_enter += [&](const auto& text)
        {
            on_command(text);
            _input->set_text(L"");
        };

        const auto update_position = [&](auto size)
        {
            _window->set_position(Point(size.width - 500, size.height / 2.0f));
        };
        _token_store += parent.on_size_changed += update_position;
        update_position(parent.size());
    }

    bool Console::visible() const
    {
        return _window->visible();
    }

    void Console::print(const std::wstring& text)
    {
        if (_log->text().empty())
        {
            _log->set_text(text);
        }
        else
        {
            _log->set_text(_log->text() + L"\n" + text);
        }
    }

    void Console::set_visible(bool value)
    {
        if (value == _window->visible())
        {
            return;
        }

        _input->set_text(L"");
        _window->set_visible(value);

        if (value)
        {
            _input->on_focus_requested();
        }
        else
        {   
            _input->on_focus_clear_requested();
        }
    }
}