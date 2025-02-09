#pragma once

#include <unordered_set>

#include "IConsole.h"
#include "../../Plugins/IPlugins.h"
#include "../../UI/Fonts/IFonts.h"

#include <trview.common/Event.h>
#include <trview.common/Windows/IDialogs.h>
#include <trview.common/TokenStore.h>

namespace trview
{
    class Console final : public IConsole
    {
    public:
        struct Names
        {
            static const inline std::string log = "##Log";
            static const inline std::string input = "##input";
        };

        explicit Console(const std::shared_ptr<IDialogs>& dialogs, const std::weak_ptr<IPlugins>& plugins, const std::shared_ptr<IFonts>& fonts);
        virtual ~Console() = default;

        void render() override;
        void set_number(int32_t number) override;
    private:
        static int callback(ImGuiInputTextCallbackData* data);
        bool render_console();
        void add_command(const std::string& command);
        void render_plugin_logs(const std::shared_ptr<IPlugin>& plugin);
        bool need_scroll(const std::shared_ptr<IPlugin>& plugin);

        TokenStore _token_store;
        std::string _buffer;
        std::shared_ptr<IDialogs> _dialogs;
        bool _need_focus{ false };
        bool _go_to_eol{ false };
        std::shared_ptr<IFonts> _fonts;
        std::string _id{ "Console 0" };
        std::vector<std::string> _recent_files;
        std::vector<std::string> _command_history;
        int32_t _command_history_index{ 0 };
        std::weak_ptr<IPlugins> _plugins;
        std::weak_ptr<IPlugin> _selected_plugin;
        std::unordered_set<IPlugin*> _need_scroll;
        std::optional<std::string> _initial_directory;
    };
}
