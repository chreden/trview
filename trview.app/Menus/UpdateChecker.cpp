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
    }

    void UpdateChecker::check_for_updates()
    {
        // Start a thread.
        _thread = std::thread(
            []()
        {
            // Update check test code.
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
            auto url = json["html_url"].get<std::string>();

            ShellExecute(0, 0, to_utf16(url).c_str(), 0, 0, SW_SHOW);
        });
    }

    void UpdateChecker::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_COMMAND:
        {

        }
        }
    }
}
