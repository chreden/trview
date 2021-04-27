#pragma once

#include <string>
#include "IClipboard.h"

namespace trview
{
    class Window;
    class Clipboard final : public IClipboard
    {
    public:
        virtual ~Clipboard() = default;
        virtual std::wstring read(const Window& window) const override;
        virtual void write(const Window& window, const std::wstring& text) override;
    };
}
