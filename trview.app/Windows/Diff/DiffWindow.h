#pragma once

#include "IDiffWindow.h"

namespace trview
{
    class DiffWindow final : public IDiffWindow
    {
    public:
        struct Names
        {
        };

        explicit DiffWindow();
        virtual ~DiffWindow() = default;
        virtual void render() override;
        virtual void set_number(int32_t number) override;
    private:
        bool render_diff_window();

        std::string _id{ "Diff 0" };
    };
}
