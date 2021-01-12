#include "UpdateChecker.h"
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        const DWORD id_update_available = 30000;
    }

    UpdateChecker::UpdateChecker(const Window& window)
        : MessageHandler(window)
    {
        TCHAR filename[MAX_PATH];
        GetModuleFileName(NULL, filename, MAX_PATH);

        DWORD handle = 0;
        std::vector<uint8_t> data(GetFileVersionInfoSize(filename, &handle), 0);
        GetFileVersionInfo(filename, 0, static_cast<uint32_t>(data.size()), &data[0]);

        LPBYTE buffer = nullptr;
        uint32_t value_size = 0;
        VerQueryValue(&data[0], L"\\", reinterpret_cast<void**>(&buffer), &value_size);
        VS_FIXEDFILEINFO* info = reinterpret_cast<VS_FIXEDFILEINFO*>(buffer);

        _current_version = "v" + std::to_string(info->dwFileVersionMS >> 16 & 0xffff) + "." +
                                 std::to_string(info->dwFileVersionMS >> 0 & 0xffff) + "." +
                                 std::to_string(info->dwFileVersionLS >> 16 & 0xffff);
    }

    UpdateChecker::~UpdateChecker()
    {
        _thread.join();
    }

    void UpdateChecker::check_for_updates()
    {
        if (_thread.joinable())
        {
            _thread.join();
        }

        _thread = std::thread(
            [](Window window, const std::string& version)
        {
            HINTERNET internet = WinHttpOpen(L"trview", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
            if (!internet)
            {
                return;
            }

            HINTERNET connect = WinHttpConnect(internet, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
            if (!connect)
            {
                return;
            }

            HINTERNET request = WinHttpOpenRequest(connect, nullptr, L"repos/chreden/trview/releases/latest", nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            if (!request)
            {
                return;
            }

            if (!WinHttpSendRequest(request, nullptr, 0, nullptr, 0, 0, 0))
            {
                return;
            }

            if (!WinHttpReceiveResponse(request, nullptr))
            {
                return;
            }

            std::vector<uint8_t> all_data;

            DWORD bytesAvailable = 0;
            while (WinHttpQueryDataAvailable(request, &bytesAvailable))
            {
                std::vector<uint8_t> data(bytesAvailable, 0);
                if (!bytesAvailable)
                {
                    break;
                }
                DWORD bytesRead = 0;
                WinHttpReadData(request, &data[0], bytesAvailable, &bytesRead);
                std::copy(data.begin(), data.begin() + bytesRead, std::back_inserter(all_data));
            }

            std::string text(all_data.begin(), all_data.end());

            try
            {
                auto json = nlohmann::json::parse(text);
                auto tag = json["tag_name"].get<std::string>();

                if (tag != version)
                {
                    HMENU menu = GetMenu(window);
                    AppendMenu(menu, MF_STRING, id_update_available, L"Update Available!");
                    DrawMenuBar(window);
                }
            }
            catch (...)
            {
            }

        }, window(), _current_version);
    }

    void UpdateChecker::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == id_update_available)
        {
            ShellExecute(0, 0, L"https://github.com/chreden/trview/releases/latest", 0, 0, SW_SHOW);
        }
    }
}
