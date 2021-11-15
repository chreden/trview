#pragma once

#include <cstdint>

namespace trview
{
    struct Resource
    {
        uint8_t* data;
        uint32_t size;
    };

    /// <summary>
    /// Get the data for the specified resource.
    /// </summary>
    /// <param name="resource_id">The integer identifier of the resource to load.</param>
    /// <param name="resource_type">The type of resource to load.</param>
    /// <returns></returns>
    Resource get_resource_memory(int resource_id, const wchar_t* resource_type);
}


