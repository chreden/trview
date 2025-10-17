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
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void render() override;
        std::weak_ptr<IStaticsWindow> create_window() override;
        void update(float delta) override;
        void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void select_static(const std::weak_ptr<IStaticMesh>& static_mesh) override;
    private:
        IStaticsWindow::Source _source;
        std::vector<std::weak_ptr<IStaticMesh>> _statics;
        std::weak_ptr<IRoom> _current_room;
        std::weak_ptr<IStaticMesh> _selected_static;
    };
}
