#pragma once

#include <external/boost/di.hpp>
#include "LevelTextureStorage.h"
#include "MeshStorage.h"
#include "SectorHighlight.h"
#include "SelectionRenderer.h"
#include "TextureStorage.h"

namespace trview
{
    auto register_app_graphics_module() noexcept
    {
        using namespace boost;
        using namespace graphics;
        return di::make_injector(
            di::bind<ILevelTextureStorage::Source>.to(
                [](const auto& injector) -> ILevelTextureStorage::Source
                {
                    return [&](auto&& level)
                    {
                        return std::make_unique<LevelTextureStorage>(
                            injector.create<std::shared_ptr<IDevice>>(),
                            injector.create<std::unique_ptr<ITextureStorage>>(),
                            level);
                    };
                }),
            di::bind<IMeshStorage::Source>.to(
                [](const auto& injector) -> IMeshStorage::Source
                {
                    return [&](auto&& level, auto&& level_texture_storage)
                    {
                        return std::make_unique<MeshStorage>(
                            injector.create<IMesh::Source>(),
                            level,
                            level_texture_storage);
                    };
                }),
            di::bind<ISectorHighlight>.to<SectorHighlight>(),
            di::bind<ISelectionRenderer>.to<SelectionRenderer>(),
            di::bind<ITextureStorage>.to<TextureStorage>()
        );
    }
}