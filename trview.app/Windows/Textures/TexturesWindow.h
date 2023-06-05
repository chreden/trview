#pragma once

#include "ITexturesWindow.h"
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/IFiles.h>

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

        explicit TexturesWindow(const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        virtual ~TexturesWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
        virtual void set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage) override;
    private:
        bool render_textures_window();
        void clamp_index();

        std::shared_ptr<IDialogs> _dialogs;
        std::shared_ptr<IFiles> _files;
        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::string _id{ "Textures 0" };
        int32_t _index{ 0u };
        bool _transparency{ true };
    };
}
