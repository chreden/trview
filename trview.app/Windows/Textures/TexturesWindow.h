#pragma once

#include "../IWindow.h"
#include <trview.common/Messages/IMessageSystem.h>
#include "../../Graphics/ILevelTextureStorage.h"

namespace trview
{
    class TexturesWindow final : public IWindow, public IRecipient, public std::enable_shared_from_this<IRecipient>
    {
    public:
        struct Names
        {
            static inline const std::string transparency_checkbox = "Transparency";
            static inline const std::string tile = "Tile";
        };

        explicit TexturesWindow(const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~TexturesWindow() = default;
        void initialise();
        void render() override;
        void update(float delta) override;
        void set_number(int32_t number) override;
        void receive_message(const Message& message) override;
        void set_texture_storage(const std::shared_ptr<ILevelTextureStorage>& texture_storage);
    private:
        bool render_textures_window();
        void clamp_index();

        std::weak_ptr<IMessageSystem> _messaging;
        std::shared_ptr<ILevelTextureStorage> _texture_storage;
        std::string _id{ "Textures 0" };
        int32_t _index{ 0u };
        bool _transparency{ true };
    };
}
