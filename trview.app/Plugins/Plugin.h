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
        explicit Plugin(const std::shared_ptr<IFiles>& files,
            std::unique_ptr<ILua> lua,
            const std::string& path);
        virtual ~Plugin() = default;
        std::string name() const override;
        std::string author() const override;
        std::string description() const override;
        void initialise(IApplication* application) override;
    private:
        std::unique_ptr<ILua> _lua;
        std::string _name{ "Unknown" };
        std::string _author{ "Unknown" };
        std::string _description;
        std::string _script;
        std::vector<std::string> _messages;
        TokenStore _token_store;
    };
}
