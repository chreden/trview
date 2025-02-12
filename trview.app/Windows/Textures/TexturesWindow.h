#pragma once

#include "ITexturesWindow.h"
#include "../../Graphics/ILevelTextureStorage.h"

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
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_number(int32_t number) override;
    private:
        bool render_textures_window();

        struct SubWindow
        {
            std::weak_ptr<ILevel> _level;
            int32_t _index{ 0u };
            bool _transparency{ true };

            void render(int index);
        };

        std::string _id{ "Textures 0" };
        std::vector<SubWindow> _sub_windows;
    };
}
