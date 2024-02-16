#pragma once

#include "ITexturesWindow.h"

namespace trview
{
    class TexturesWindow final : public ITexturesWindow
    {
    public:
        struct Names
        {
            static inline const std::string transparency_checkbox = "Transparency";
            static inline const std::string tile = "Tile";
        };

        virtual ~TexturesWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
        virtual void set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage) override;
    private:
        bool render_textures_window();
        void clamp_index();
        void clamp_remastered_index();
        void render_og_textures();
        void render_remastered_textures();

        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::string _id{ "Textures 0" };
        int32_t _index{ 0u };
        int32_t _remastered_index{ 0u };
        bool _transparency{ true };
    };
}
