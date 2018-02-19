#pragma once

#include <cstdint>
#include <vector>
#include <memory>

#include <trview.common/Texture.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Image;
        class Window;
        class Label;
    }

    class TextureWindow
    {
    public:
        explicit TextureWindow(ui::Control* parent);
        void     set_textures(std::vector<Texture> textures);
        void     cycle();
        void     cycle_back();
        void     toggle_visibility();
        bool     visible() const;
        void     set_visible(bool value);
    private:
        void update_texture_status();

        std::vector<Texture>  _level_textures;
        uint32_t              _texture_index{ 0u };
        uint32_t              _x { 10u };
        uint32_t              _y { 10u };

        ui::Window*           _window;
        ui::Image*            _image;
        ui::Label*            _label;
    };
}