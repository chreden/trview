#include "Resources.h"

namespace trview
{
    namespace
    {
        Resource get_resource_memory_internal(const wchar_t* resource_id, const wchar_t* resource_type)
        {
            HMODULE module = GetModuleHandle(NULL);
            HRSRC resource = FindResource(module, resource_id, resource_type);
            HGLOBAL memory = LoadResource(module, resource);
            DWORD size = SizeofResource(module, resource);
            LPVOID data = LockResource(memory);
            return Resource{ static_cast<uint8_t*>(data), static_cast<uint32_t>(size) };
        }
    }

    Resource get_resource_memory(int resource_id, const wchar_t* resource_type)
    {
        return get_resource_memory_internal(MAKEINTRESOURCE(resource_id), resource_type);
    }

    Resource get_resource_memory(const std::wstring& resource_name, const wchar_t* resource_type)
    {
        return get_resource_memory_internal(resource_name.c_str(), resource_type);
    }

    std::vector<std::wstring> get_resource_names_of_type(const std::wstring& type)
    {
        const auto enum_names_func = [](HMODULE, LPCWSTR, LPWSTR name, LONG_PTR param) -> BOOL
        {
            if (IS_INTRESOURCE(name))
            {
                return FALSE;
            }
            reinterpret_cast<std::vector<std::wstring>*>(param)->push_back(name);
            return TRUE;
        };

        std::vector<std::wstring> names;
        EnumResourceNames(GetModuleHandle(nullptr), type.c_str(), enum_names_func, reinterpret_cast<LONG_PTR>(&names));
        return names;
    }
}
