#pragma once

#include <cstdint>
#include <trlevel/ILevel.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>

namespace trview
{
    class Mesh;

    struct IMeshStorage
    {
        using Source = std::function<std::unique_ptr<IMeshStorage>(const trlevel::ILevel&, const ILevelTextureStorage&)>;

        virtual ~IMeshStorage() = 0;

        virtual Mesh* mesh(uint32_t mesh_pointer) const = 0;
    };
}
