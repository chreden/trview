#pragma once

#include <vector>

#include <trview.common/Windows/IClipboard.h>

#include "IStaticsWindow.h"
#include "../ColumnSizer.h"
#include "../../Elements/IRoom.h"
#include "../../Elements/IStaticMesh.h"
#include "../../Filters/Filters.h"
#include "../../Track/Track.h"
#include "../AutoHider.h"

namespace trview
{
    class StaticsWindow final : public IStaticsWindow
    {
    public:
        struct Names
        {
            static inline const std::string sync_item = "Sync";
            static inline const std::string statics_list = "##staticslist";
            static inline const std::string statics_list_panel = "Statics List";
            static inline const std::string details_panel = "Static Details";
            static inline const std::string static_stats = "##staticstats";
        };

        explicit StaticsWindow(const std::shared_ptr<IClipboard>& clipboard);
        virtual ~StaticsWindow() = default;
        void add_level(const std::weak_ptr<ILevel>& level) override;
        void render() override;
        void set_current_room(const std::weak_ptr<IRoom>& room) override;
        void set_number(int32_t number) override;
        void set_selected_static(const std::weak_ptr<IStaticMesh>& static_mesh) override;
        void update(float dt) override;
    private:
        bool render_statics_window();

        struct SubWindow
        {
            Event<std::weak_ptr<IStaticMesh>> on_static_selected;

            std::vector<std::weak_ptr<IStaticMesh>> _all_statics;
            AutoHider _auto_hider;
            std::shared_ptr<IClipboard> _clipboard;
            ColumnSizer _column_sizer;
            std::weak_ptr<IRoom> _current_room;
            Filters<IStaticMesh> _filters;
            bool _force_sort{ false };
            std::weak_ptr<IStaticMesh> _global_selected_static;
            std::weak_ptr<ILevel> _level;
            bool _scroll_to_static{ false };
            std::weak_ptr<IStaticMesh> _selected_static_mesh;
            bool _sync_static{ true };
            Track<Type::Room> _track;

            void calculate_column_widths();
            void render(int index);
            void render_statics_list();
            void render_static_details();
            void setup_filters();
            void set_current_room(const std::weak_ptr<IRoom>& room);
            void set_local_selected_static_mesh(std::weak_ptr<IStaticMesh> static_mesh);
            void set_selected_static(const std::weak_ptr<IStaticMesh>& static_mesh);
            void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics);
            void set_sync_static(bool value);
        };

        std::string _id{ "Statics 0" };
        std::vector<SubWindow> _sub_windows;
        std::shared_ptr<IClipboard> _clipboard;
    };
}

