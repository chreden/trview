#include "Lua_Matrix.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int matrix_metatable = LUA_NOREF;

            int matrix_constructor(lua_State* L)
            {
                return create_matrix(L, Matrix::Identity);
            }

            int matrix_index(lua_State* L)
            {
                const auto& self = get_userdata<Matrix>(L, 1);
                const std::string key = lua_tostring(L, 2);
                self;
                key;
                return 0;
            }

            int matrix_gc(lua_State* L)
            {
                cleanup_userdata<Matrix>(L, 1);
                return 0;
            }

            int matrix_mul(lua_State* L)
            {
                const auto& self = get_userdata<Matrix>(L, 1);
                if (is_matrix(L, 2))
                {
                    return create_matrix(L, self * get_userdata<Matrix>(L, 2));
                }
                return 0;
            }

            int matrix_rotation_x(lua_State* L)
            {
                return create_matrix(L, Matrix::CreateRotationX(static_cast<float>(lua_tonumber(L, 1))));
            }

            int matrix_rotation_y(lua_State* L)
            {
                return create_matrix(L, Matrix::CreateRotationY(static_cast<float>(lua_tonumber(L, 1))));
            }

            int matrix_rotation_z(lua_State* L)
            {
                return create_matrix(L, Matrix::CreateRotationZ(static_cast<float>(lua_tonumber(L, 1))));
            }

            int matrix_class_index(lua_State* L)
            {
                const std::string key = lua_tostring(L, 2);
                if (key == "rotationX")
                {
                    lua_pushcfunction(L, matrix_rotation_x);
                    return 1;
                }
                else if (key == "rotationY")
                {
                    lua_pushcfunction(L, matrix_rotation_y);
                    return 1;
                }
                else if (key == "rotationZ")
                {
                    lua_pushcfunction(L, matrix_rotation_z);
                    return 1;
                }
                return 0;
            }
        }

        int create_matrix(lua_State* L, const Matrix& value)
        {
            create_userdata(L, value);
            assign_metatable(L, matrix_metatable);
            return 1;
        }

        bool is_matrix(lua_State* L, int index)
        {
            return equal_metatable(L, index, matrix_metatable);
        }

        void matrix_register(lua_State* L)
        {
            matrix_metatable = store_metatable(L,
                {
                    { "__index", matrix_index },
                    { "__gc", matrix_gc },
                    { "__mul", matrix_mul }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", matrix_constructor },
                    { "__index", matrix_class_index }
                });
            lua_setglobal(L, "Matrix");
        }
    }
}
