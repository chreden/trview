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
    /// <returns>The resource.</returns>
    Resource get_resource_memory(int resource_id, const wchar_t* resource_type);
    /// <summary>
    /// Get the data for the specified resource.
    /// </summary>
    /// <param name="resource_name">The name of the resource to load.</param>
    /// <param name="resource_type">The type of the resource to load.</param>
    /// <returns>The resource.</returns>
    Resource get_resource_memory(const std::wstring& resource_name, const wchar_t* resource_type);
    /// <summary>
    /// Get all resources of the specified type.
    /// </summary>
    /// <param name="type">The type to search for.</param>
    /// <returns>The resource names.</returns>
    std::vector<std::wstring> get_resource_names_of_type(const std::wstring& type);
}


