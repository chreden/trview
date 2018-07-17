#include "stdafx.h"
#include "ResourceHelper.h"

namespace trview
{
    Resource get_resource_memory(int resource_id, const wchar_t* resource_type)
    {
        HMODULE module = GetModuleHandle(NULL);
        HRSRC resource = FindResource(module, MAKEINTRESOURCE(resource_id), resource_type);
        HGLOBAL memory = LoadResource(module, resource);
        DWORD size = SizeofResource(module, resource);
        LPVOID data = LockResource(memory);
        return Resource{ static_cast<uint8_t*>(data), static_cast<uint32_t>(size) };
    }
}
