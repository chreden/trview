#pragma once

#include "IDialogs.h"

namespace trview
{
    class Dialogs final : public IDialogs
    {
    public:
        virtual ~Dialogs() = default;
        virtual bool message_box(const Window& window, const std::wstring& message, const std::wstring& title, Buttons buttons) const override;
        virtual std::optional<std::string> open_file(const std::wstring& title, const std::wstring& filter, const std::vector<std::wstring>& file_types, uint32_t flags) const override;
        virtual std::optional<std::string> save_file(const std::wstring& title, const std::wstring& filter, const std::vector<std::wstring>& file_types) const override;
    };
}
