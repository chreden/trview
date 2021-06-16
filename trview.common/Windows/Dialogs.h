#pragma once

#include "IDialogs.h"

namespace trview
{
    class Dialogs final : public IDialogs
    {
    public:
        virtual ~Dialogs() = default;
        virtual bool message_box(const Window& window, const std::wstring& title, const std::wstring& message, Buttons buttons) override;
    };
}
