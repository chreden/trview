#pragma once

#include <external/lua/src/lua.hpp>

#include "IPlugin.h"

namespace trview
{
    class Plugin final : public IPlugin
    {
    public:
        explicit Plugin(const std::shared_ptr<ILog>& log, const std::string& name, const std::vector<uint8_t>& data);
        virtual ~Plugin();
        virtual void initialise() override;
        virtual void render() override;
        virtual void render_ui() override;
    private:
        void error(const std::string& activity);

        std::shared_ptr<ILog> _log;
        std::string _name;
        std::vector<uint8_t> _data;
        lua_State* _state{ nullptr };
        Status _status{ Status::OK };
    };
}
