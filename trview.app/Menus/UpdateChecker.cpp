#include "UpdateChecker.h"
#include <WinHttp.h>
#include <external/nlohmann/json.hpp>
#include <trview.common/Strings.h>
#include <shellapi.h>

namespace trview
{
    UpdateChecker::UpdateChecker(const Window& window)
        : MessageHandler(window)
    {
        TCHAR filename[MAX_PATH];
        GetModuleFileName(NULL, filename, MAX_PATH);
        
        DWORD handle = 0;
        auto size = GetFileVersionInfoSize(filename, &handle);
        
        std::vector<uint8_t> data(size, 0);
        GetFileVersionInfo(filename, 0, size, &data[0]);

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
        _thread = std::thread(
            [](HWND window, const std::string& version)
        {
            HINTERNET internet = WinHttpOpen(L"trview", WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, nullptr, nullptr, 0);
            HINTERNET connect = WinHttpConnect(internet, L"api.github.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
            HINTERNET request = WinHttpOpenRequest(connect, nullptr, L"repos/chreden/trview/releases/latest", nullptr, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
            BOOL success = WinHttpSendRequest(request, nullptr, 0, nullptr, 0, 0, 0);
            success = WinHttpReceiveResponse(request, nullptr);

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
            auto json = nlohmann::json::parse(text);
            auto tag = json["tag_name"].get<std::string>();

            if (tag != version)
            {
                HMENU menu = GetMenu(window);
                AppendMenu(menu, MF_STRING, (UINT_PTR)30000, L"Update Available!");
            }

        }, window(), _current_version);
    }

    void UpdateChecker::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == 30000)
        {
            ShellExecute(0, 0, L"https://github.com/chreden/trview/releases/latest", 0, 0, SW_SHOW);
        }
    }
}
