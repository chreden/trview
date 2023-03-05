#pragma once

#include "IDialogs.h"

namespace trview
{
    class Dialogs final : public IDialogs
    {
    public:
        explicit Dialogs(const Window& window);
        virtual ~Dialogs() = default;
        virtual bool message_box(const std::wstring& message, const std::wstring& title, Buttons buttons) const override;
        virtual std::optional<FileResult> open_file(const std::wstring& title, const std::vector<FileFilter>& filters, uint32_t flags) const override;
        std::optional<std::string> open_folder() const override;
        virtual std::optional<FileResult> save_file(const std::wstring& title, const std::vector<FileFilter>& filters, uint32_t filter_index) const override;
    private:
        Window _window;
    };
}
