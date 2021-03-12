#pragma once

#include <cstdint>

namespace trview
{
    struct Resource
    {
        uint8_t* data;
        uint32_t size;
    };

    /// Get the data for the specified resource.
    /// @param resource_id The integer identifier of the resource to load.
    /// @param resource_type The type of resource to load.
    /// @returns The resource.
    Resource get_resource_memory(int resource_id, const wchar_t* resource_type);
}

