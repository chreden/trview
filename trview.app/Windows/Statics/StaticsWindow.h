#pragma once

#include "IStaticsWindow.h"
#include "../ColumnSizer.h"

namespace trview
{
    class StaticsWindow final : public IStaticsWindow
    {
    public:
        struct Names
        {
        };

        explicit StaticsWindow();
        virtual ~StaticsWindow() = default;
        void render() override;
        void set_number(int32_t number) override;
        void update(float dt) override;
    private:
        bool render_statics_window();
        void calculate_column_widths();

        std::string _id{ "Statics 0" };
        ColumnSizer _column_sizer;
    };
}

