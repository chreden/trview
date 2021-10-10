#include "Dialogs.h"
#include "Strings.h"
#include <commdlg.h>

namespace trview
{
    namespace
    {
        long convert_button(IDialogs::Buttons buttons)
        {
            switch (buttons)
            {
                case IDialogs::Buttons::OK_Cancel:
                    return MB_OKCANCEL;
                case IDialogs::Buttons::Yes_No:
                    return MB_YESNO;
                case IDialogs::Buttons::OK:
                default:
                    return MB_OK;
            }
        }

        bool convert_response(int response)
        {
            return response == IDOK || response == IDYES;
        }

        std::wstring combine_filters(const std::wstring& filter, const std::vector<std::wstring>& file_types)
        {
            std::wstring final_filter = filter + L'\0';
            for (auto i = 0; i < file_types.size(); ++i)
            {
                final_filter += file_types[i];
                if (i < file_types.size() - 1)
                {
                    final_filter += L';';
                }
            }
            final_filter += L'\0';
            return final_filter;
        }

        std::wstring combine_filters(const std::vector<IDialogs::FileFilter>& filters)
        {
            std::wstring final_filter;
            for (const auto& filter : filters)
            {
                final_filter += filter.name + L'\0';
                for (auto i = 0; i < filter.file_types.size(); ++i)
                {
                    final_filter += filter.file_types[i];
                    if (i < filter.file_types.size() - 1)
                    {
                        final_filter += L';';
                    }
                }
                final_filter += L'\0';
            }
            final_filter += L'\0';
            return final_filter;
        }
    }

    bool Dialogs::message_box(const Window& window, const std::wstring& message, const std::wstring& title, Buttons buttons) const
    {
        return convert_response(MessageBox(window, message.c_str(), title.c_str(), convert_button(buttons)));
    }

    std::optional<std::string> Dialogs::open_file(const std::wstring& title, const std::vector<FileFilter>& filters, uint32_t flags) const
    {
        OPENFILENAME ofn;
        memset(&ofn, 0, sizeof(ofn));

        wchar_t path[MAX_PATH];
        memset(&path, 0, sizeof(path));

        const auto final_filters = combine_filters(filters);

        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFile = path;
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = title.c_str();
        ofn.lpstrFilter = final_filters.c_str();
        ofn.Flags = flags;

        if (GetOpenFileName(&ofn))
        {
            return to_utf8(ofn.lpstrFile);
        }
        return {};
    }

    std::optional<IDialogs::SaveFileResult> Dialogs::save_file(const std::wstring& title, const std::vector<FileFilter>& filters) const
    {
        OPENFILENAME ofn;
        memset(&ofn, 0, sizeof(ofn));

        wchar_t path[MAX_PATH];
        memset(&path, 0, sizeof(path));

        const auto final_filters = combine_filters(filters);

        ofn.lStructSize = sizeof(ofn);
        ofn.lpstrFilter = final_filters.c_str();
        ofn.nMaxFile = MAX_PATH;
        ofn.lpstrTitle = title.c_str();
        ofn.lpstrFile = path;
        ofn.lpstrDefExt = L"0";

        if (GetSaveFileName(&ofn))
        {
            return IDialogs::SaveFileResult { trview::to_utf8(ofn.lpstrFile), static_cast<int>(ofn.nFilterIndex) };
        }
        return {};
    }
}
