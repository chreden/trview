#pragma once

#include <external/boost/di.hpp>
#include "Mesh.h"
#include "Picking.h"
#include "TransparencyBuffer.h"

namespace trview
{
    auto register_app_geometry_module() noexcept
    {
        using namespace boost;
        using namespace graphics;
        return di::make_injector(
            di::bind<IMesh::Source>.to(
                [](const auto& injector) -> IMesh::Source
                {
                    return [&](auto&& vertices, auto&& indices, auto&& untextured_indices, auto&& transparent_triangles, auto&& collision_triangles)
                    {
                        return std::make_shared<Mesh>(
                            injector.create<std::shared_ptr<IDevice>>(),
                            vertices,
                            indices,
                            untextured_indices,
                            transparent_triangles,
                            collision_triangles);
                    };
                }),
            di::bind<IMesh::TransparentSource>.to(
                [](auto&&) -> IMesh::TransparentSource
                {
                    return [&](auto&& transparent_triangles, auto&& collision_triangles)
                    {
                        return std::make_shared<Mesh>(transparent_triangles, collision_triangles);
                    };
                }),
            di::bind<IPicking>.to<Picking>(),
            di::bind<ITransparencyBuffer>.to<TransparencyBuffer>()
        );
    }
}