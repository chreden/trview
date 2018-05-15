#include "stdafx.h"
#include "TextureWindow.h"
#include <sstream>

#include <trview.ui/Window.h>
#include <trview.ui/Label.h>
#include <trview.ui/Image.h>

namespace trview
{
    TextureWindow::TextureWindow(ui::Control* parent)
    {
        auto texture_window = std::make_unique<ui::Window>(
            ui::Point(300, 10),
            Size(260, 300),
            ui::Colour(1.0f, 0.0f, 0.0f, 0.0f));

        auto image = std::make_unique<ui::Image>(
            ui::Point(2, 2),
            Size(256, 256));

        auto label = std::make_unique<ui::Label>(
            ui::Point(0, 260),
            Size(260, 40),
            ui::Colour(1.0f, 0.1f, 0.1f, 0.1f),
            L"No level loaded");

        _image = image.get();
        _label = label.get();
        _window = texture_window.get();

        texture_window->add_child(std::move(image));
        texture_window->add_child(std::move(label));
        parent->add_child(std::move(texture_window));
    }

    void TextureWindow::set_textures(std::vector<graphics::Texture> textures)
    {
        _texture_index = 0u;
        _level_textures = textures;
        update_texture_status();
    }

    void TextureWindow::cycle()
    {
        ++_texture_index;
        if (_texture_index >= _level_textures.size())
        {
            _texture_index = 0;
        }
        update_texture_status();
    }

    void TextureWindow::cycle_back()
    {
        if (_texture_index == 0)
        {
            _texture_index = static_cast<uint32_t>(_level_textures.size()) - 1;
        }
        else
        {
            --_texture_index;
        }
        update_texture_status();
    }

    void TextureWindow::update_texture_status()
    {
        std::wstringstream stream;
        stream << L"Texture " << _texture_index + 1 << L"/" << _level_textures.size();
        _label->set_text(stream.str());

        if (_texture_index < _level_textures.size())
        {
            _image->set_texture(_level_textures[_texture_index]);
        }
    }

    void TextureWindow::toggle_visibility()
    {
        _window->set_visible(!visible());
    }

    bool TextureWindow::visible() const
    {
        return _window->visible();
    }

    void TextureWindow::set_visible(bool value)
    {
        _window->set_visible(value);
    }
}