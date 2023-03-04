#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/IFiles.h>
#include "IPlugin.h"
#include "../Lua/ILua.h"

namespace trview
{
    struct Plugin final : public IPlugin
    {
    public:
        explicit Plugin(std::unique_ptr<ILua> lua, const std::string& name, const std::string& author, const std::string& description);
        explicit Plugin(const std::shared_ptr<IFiles>& files,
            std::unique_ptr<ILua> lua,
            const std::string& path);
        virtual ~Plugin() = default;
        std::string name() const override;
        std::string author() const override;
        std::string description() const override;
        void initialise(IApplication* application) override;
        std::string path() const override;
        std::string messages() const override;
        void execute(const std::string& command) override;
        void add_message(const std::string& message) override;
        void do_file(const std::string& path) override;
        void clear_messages() override;
    private:
        void register_print();

        std::unique_ptr<ILua> _lua;
        std::string _path;
        std::string _name{ "Unknown" };
        std::string _author{ "Unknown" };
        std::string _description;
        std::string _script;
        std::string _messages;
        TokenStore _token_store;
    };
}
