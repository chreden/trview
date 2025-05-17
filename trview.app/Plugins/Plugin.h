#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/IFiles.h>
#include "IPlugin.h"
#include "../Lua/ILua.h"

namespace trview
{
    class Plugin final : public IPlugin
    {
    public:
        explicit Plugin(std::unique_ptr<ILua> lua, const std::string& name, const std::string& author, const std::string& description);
        explicit Plugin(const std::shared_ptr<IFiles>& files,
            std::unique_ptr<ILua> lua,
            const std::string& path);
        virtual ~Plugin() = default;
        bool built_in() const override;
        std::string name() const override;
        std::string author() const override;
        std::string description() const override;
        bool enabled() const override;
        void initialise(IApplication* application) override;
        std::string path() const override;
        std::string messages() const override;
        void execute(const std::string& command) override;
        void add_message(const std::string& message) override;
        void do_file(const std::string& path) override;
        void clear_messages() override;
        void reload() override;
        void render_context_menu(const PickResult& pick_result) override;
        void render_toolbar() override;
        void render_ui() override;
        void set_enabled(bool value) override;
    private:
        void load();
        void load_script();
        void register_print();
        void set_package_path();

        std::shared_ptr<IFiles> _files;
        std::unique_ptr<ILua> _lua;
        std::string _path;
        std::string _name{ "Unknown" };
        std::string _author{ "Unknown" };
        std::string _description;
        std::string _script;
        std::string _messages;
        TokenStore _token_store;
        IApplication* _application;
        bool _enabled{ true };
        bool _built_in{ false };
        bool _script_loaded{ false };
    };
}
