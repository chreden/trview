#pragma once

#include <unordered_map>
#include <trview.common/Windows/IShortcuts.h>
#include <trview.common/TokenStore.h>
#include <trview.common/MessageHandler.h>

#include "IWindows.h"
#include "IWindow.h"

namespace trview
{
    class Windows final : public IWindows, public MessageHandler
    {
    public:
        explicit Windows(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts);
        virtual ~Windows() = default;
        std::weak_ptr<IWindow> create(const std::string& type) override;
        void update(float elapsed) override;
        std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        void register_window(const std::string& type, const Creator& creator) override;
        void render() override;
        void setup(const UserSettings& settings) override;
        std::vector<std::weak_ptr<IWindow>> windows(const std::string& type) const override;
    private:
        std::weak_ptr<IWindow> add_window(const std::shared_ptr<IWindow>& window);

        /// <summary>
        /// Find the next ID.
        /// </summary>
        /// <returns>The next available window number.</returns>
        int32_t next_id(const std::string& type) const;

        std::unordered_map<std::string, Creator> _creators;
        std::vector<std::pair<std::string, int32_t>> _closing_windows;
        TokenStore _token_store;
        std::unordered_map<std::string, std::unordered_map<int32_t, std::shared_ptr<IWindow>>> _windows;
    };
}
