#pragma once

#include "IWindows.h"

namespace trview
{
    struct ICameraSinkWindowManager;
    struct IPluginsWindowManager;
    struct IStaticsWindowManager;

    class Windows final : public IWindows
    {
    public:
        explicit Windows(
            std::unique_ptr<ICameraSinkWindowManager> camera_sink_windows,
            std::unique_ptr<IPluginsWindowManager> plugins_window_manager,
            std::unique_ptr<IStaticsWindowManager> statics_window_manager);
        virtual ~Windows() = default;
        void update(float elapsed) override;
        void render() override;
        void select(const std::weak_ptr<ICameraSink>& camera_sink) override;
        void select(const std::weak_ptr<IStaticMesh>& static_mesh) override;
        void set_level(const std::weak_ptr<ILevel>& level) override;
        void set_room(const std::weak_ptr<IRoom>& room) override;
        void setup(const UserSettings& settings) override;
    private:
        std::unique_ptr<ICameraSinkWindowManager> _camera_sink_windows;
        std::unique_ptr<IPluginsWindowManager> _plugins_windows;
        std::unique_ptr<IStaticsWindowManager> _statics_windows;
    };
}
