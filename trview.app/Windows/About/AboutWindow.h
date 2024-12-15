#pragma once

#include "IAboutWindow.h"

namespace trview
{
    class AboutWindow final : public IAboutWindow
    {
    public:
        virtual ~AboutWindow() = default;
        void focus() override;
        void render() override;
    private:
        bool _need_focus{ false };
    };
}
