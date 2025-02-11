#pragma once

#include <minwindef.h>
#include <memory>
#include <optional>

#include <trview.common/Window.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/IShortcuts.h>

#include "../WindowManager.h"
#include "IStaticsWindowManager.h"
#include "IStaticsWindow.h"

namespace trview
{
    class StaticsWindowManager final : public IStaticsWindowManager, public WindowManager<IStaticsWindow>, public MessageHandler
    {
    public:
        explicit StaticsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IStaticsWindow::Source& statics_window_source);
        ~StaticsWindowManager() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        std::weak_ptr<IStaticsWindow> create_window() override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        void select_static(const std::weak_ptr<IStaticMesh>& static_mesh) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void update(float delta) override;
    private:
        struct Selection
        {
            std::weak_ptr<ILevel> level;
            std::weak_ptr<IRoom> room;
            std::weak_ptr<IStaticMesh> static_mesh;
        };
        std::vector<Selection> _levels;
        IStaticsWindow::Source _source;
    };
}
