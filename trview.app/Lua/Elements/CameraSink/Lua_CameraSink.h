#pragma once

#include "../../../Elements/CameraSink/ICameraSink.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void create_camera_sink(lua_State* L, std::shared_ptr<ICameraSink> camera_sink);
    }
}
