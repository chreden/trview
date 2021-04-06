#pragma once

#include <cstdint>
#include <trlevel/ILevel.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>

namespace trview
{
    struct IMesh;

    struct IMeshStorage
    {
        using Source = std::function<std::unique_ptr<IMeshStorage>(const trlevel::ILevel&, const ILevelTextureStorage&)>;

        virtual ~IMeshStorage() = 0;

        virtual IMesh* mesh(uint32_t mesh_pointer) const = 0;
    };
}
