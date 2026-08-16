#pragma once

#include "IViewMenu.h"

namespace trview
{
    struct IMessageSystem;
    class ViewMenu final : public IViewMenu
    {
    public:
        explicit ViewMenu(const std::weak_ptr<IMessageSystem>& messaging);
        virtual ~ViewMenu() = default;
        void render() override;
    private:
        void show(const std::string& key, bool value);

        std::weak_ptr<IMessageSystem> _messaging;
        bool _show_minimap { true };
        bool _show_tooltip { true };
        bool _show_ui { true };
        bool _show_compass { true };
        bool _show_selection { true };
        bool _show_route { true };
        bool _show_tools { true };
    };
}
