#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Messages/IRecipient.h>

#include <memory>
#include "IMainMenu.h"

namespace trview
{
    struct IFileMenu;
    struct IViewMenu;
    struct IMessageSystem;
    struct ILevelInfo;

    class MainMenu final : public IMainMenu, public IRecipient
    {
    public:
        explicit MainMenu(
            const std::weak_ptr<IMessageSystem>& messaging,
            const std::shared_ptr<IFileMenu>& file_menu,
            const std::shared_ptr<IViewMenu>& view_menu,
            const std::shared_ptr<ILevelInfo>& level_info);
        virtual ~MainMenu() = default;
        void render() override;
        void receive_message(const Message& message) override;
    private:
        std::shared_ptr<IFileMenu> _file_menu;
        TokenStore _token_store;
        std::weak_ptr<IMessageSystem> _messaging;
        std::shared_ptr<IViewMenu> _view_menu;
        std::shared_ptr<ILevelInfo> _level_info;
        bool _show_update_available{ false };
    };
}
