#pragma once

#include "IStaticsWindow.h"
#include "../ColumnSizer.h"
#include "../../Elements/IStaticMesh.h"
#include "../../Filters/Filters.h"

namespace trview
{
    class StaticsWindow final : public IStaticsWindow
    {
    public:
        struct Names
        {
            static inline const std::string statics_list = "##staticslist";
            static inline const std::string statics_list_panel = "Statics List";
        };

        explicit StaticsWindow();
        virtual ~StaticsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void update(float dt) override;
        void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics) override;
    private:
        bool render_statics_window();
        void render_statics_list();
        void render_static_details();
        void calculate_column_widths();
        void setup_filters();

        std::string _id{ "Statics 0" };
        ColumnSizer _column_sizer;
        std::vector<std::weak_ptr<IStaticMesh>> _all_statics;
        bool _force_sort{ false };
        Filters<IStaticMesh> _filters;
        bool _scroll_to_static{ false };
    };
}

