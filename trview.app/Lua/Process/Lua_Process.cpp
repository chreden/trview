#include "Lua_Process.h"
#include "../Lua.h"

#include <tlhelp32.h>

namespace trview
{
    namespace lua
    {
        int find_process(lua_State* L)
        {
            std::wstring name = to_utf16(lua_tostring(L, -1));

            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (Process32First(snapshot, &entry))
            {
                while (Process32Next(snapshot, &entry))
                {
                    if (entry.szExeFile == name)
                    {
                        CloseHandle(snapshot);
                        lua_pushnumber(L, entry.th32ProcessID);
                        return 1;
                    }
                }
            }

            CloseHandle(snapshot);
            lua_pushnil(L);
            return 1;
        }

        int read_int32(lua_State* L)
        {
            uint32_t pid = static_cast<uint32_t>(lua_tointeger(L, 1));
            uint64_t offset = lua_tointeger(L, 2);

            HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, pid);
            int output = 0;
            ReadProcessMemory(process, (void*)offset, &output, sizeof(int), nullptr);

            CloseHandle(process);

            lua_pushnumber(L, output);
            return 1;
        }

        int read_int64(lua_State* L)
        {
            uint32_t pid = static_cast<uint32_t>(lua_tointeger(L, 1));
            uint64_t offset = lua_tointeger(L, 2);

            HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, pid);
            int64_t output = 0;
            ReadProcessMemory(process, (void*)offset, &output, sizeof(int64_t), nullptr);

            CloseHandle(process);

            lua_pushnumber(L, static_cast<lua_Number>(output));
            return 1;
        }

        int read_int16(lua_State* L)
        {
            uint32_t pid = static_cast<uint32_t>(lua_tointeger(L, 1));
            uint64_t offset = lua_tointeger(L, 2);

            HANDLE process = OpenProcess(PROCESS_VM_READ, FALSE, pid);
            int16_t output = 0;
            ReadProcessMemory(process, (void*)offset, &output, sizeof(int16_t), nullptr);

            CloseHandle(process);

            lua_pushnumber(L, output);
            return 1;
        }

        void process_register(lua_State* L)
        {
            lua_newtable(L);
            lua_pushcfunction(L, find_process);
            lua_setfield(L, -2, "find");
            lua_pushcfunction(L, read_int16);
            lua_setfield(L, -2, "read_int16");
            lua_pushcfunction(L, read_int32);
            lua_setfield(L, -2, "read_int32");
            lua_pushcfunction(L, read_int64);
            lua_setfield(L, -2, "read_int64");
            lua_setglobal(L, "Process");
        }
    }
}