#pragma once

#include "../../Camera/ICamera.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void camera_register(lua_State* L);
        int create_camera(lua_State* L, const std::shared_ptr<ICamera>& camera);
        std::shared_ptr<ICamera> to_camera(lua_State* L, int index);
    }
}
