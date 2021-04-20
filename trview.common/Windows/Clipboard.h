#pragma once

#include <string>
#include "IClipboard.h"

namespace trview
{
    class Window;

    std::wstring read_clipboard(const Window& window);
    void write_clipboard(const Window& window, const std::wstring& text);

    class Clipboard final : public IClipboard
    {
    public:
        virtual ~Clipboard() = default;
        virtual std::wstring read(const Window& window) const override;
        virtual void write(const Window& window, const std::wstring& text) override;
    };
}
