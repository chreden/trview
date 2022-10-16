#pragma once

#include "IClipboard.h"

namespace trview
{
    class Clipboard final : public IClipboard
    {
    public:
        explicit Clipboard(const Window& window);
        virtual ~Clipboard() = default;
        virtual std::wstring read() const override;
        virtual void write(const std::wstring& text) override;
    private:
        Window _window;
    };
}
