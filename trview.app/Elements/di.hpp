#pragma once

#include <external/boost/di.hpp>
#include "Level.h"
#include "TypeNameLookup.h"
#include "Entity.h"
#include "StaticMesh.h"
#include "Sector.h"
#include "Light.h"

#include "Resources/resource.h"
#include <trview.common/Resources.h>

namespace trview
{
    auto register_app_elements_module() noexcept
    {
        using namespace boost;
        using namespace graphics;
        return di::make_injector(
            di::bind<ITypeNameLookup>.to(
                []()
                {
                    Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");
                    return std::make_shared<TypeNameLookup>(std::string(type_list.data, type_list.data + type_list.size));
                }),
            di::bind<IEntity::EntitySource>.to(
                [](const auto& injector) -> IEntity::EntitySource
                {
                    return [&](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage)
                    {
                        const auto type_names = injector.create<std::shared_ptr<ITypeNameLookup>>();
                        return std::make_shared<Entity>(injector.create<IMesh::Source>(), level, entity, mesh_storage, index, type_names->is_pickup(level.get_version(), entity.TypeID));
                    };
                }),
            di::bind<IEntity::AiSource>.to(
                [](const auto& injector) -> IEntity::AiSource
                {
                    return [&](auto&& level, auto&& ai_object, auto&& index, auto&& mesh_storage)
                    {
                        return std::make_shared<Entity>(injector.create<IMesh::Source>(), level, ai_object, mesh_storage, index);
                    };
                }),
            di::bind<IRoom::Source>.to(
                [](const auto& injector) -> IRoom::Source
                {
                    return [&](auto&& level, auto&& room, auto&& texture_storage, auto&& mesh_storage, auto&& index, auto&& parent_level)
                    {
                        return std::make_shared<Room>(injector.create<IMesh::Source>(), level, room, texture_storage, mesh_storage, index, parent_level,
                            injector.create<IStaticMesh::MeshSource>(), injector.create<IStaticMesh::PositionSource>(), injector.create<ISector::Source>());
                    };
                }),
            di::bind<ITrigger::Source>.to(
                [](const auto& injector) -> ITrigger::Source
                {
                    return [&](auto&& number, auto&& room, auto&& x, auto&& z, auto&& trigger_info)
                    {
                        return std::make_shared<Trigger>(number, room, x, z, trigger_info, injector.create<IMesh::TransparentSource>());
                    };
                }),
            di::bind<IStaticMesh::MeshSource>.to(
                [](const auto& injector) -> IStaticMesh::MeshSource
                {
                    auto bounding_mesh = create_cube_mesh(injector.create<IMesh::Source>());
                    return [=](auto&& room_mesh, auto&& level_mesh, auto&& mesh)
                    {
                        return std::make_shared<StaticMesh>(room_mesh, level_mesh, mesh, bounding_mesh);
                    };
                }),
            di::bind<IStaticMesh::PositionSource>.to(
                [](const auto&) -> IStaticMesh::PositionSource
                {
                    return [&](auto&& position, auto&& scale, auto&& mesh)
                    {
                        return std::make_shared<StaticMesh>(position, scale, mesh);
                    };
                }),
            di::bind<ISector::Source>.to(
                [](const auto&) -> ISector::Source
                {
                    return [&](auto&& level, auto&& room, auto&& room_sector, auto&& sector_id, auto&& room_number)
                    {
                        return std::make_shared<Sector>(level, room, room_sector, sector_id, room_number);
                    };
                }),
            di::bind<ILight::Source>.to(
                [](const auto& injector) -> ILight::Source
                {
                    const auto mesh = create_sphere_mesh(injector.create<IMesh::Source>(), 24, 24);
                    return [=](auto&&... args)
                    {
                        return std::make_shared<Light>(mesh, args...);
                    };
                }),
            di::bind<ILevel::Source>.to(
                [](const auto& injector) -> ILevel::Source
                {
                    return [&](auto&& level)
                    {
                        auto texture_storage = injector.create<ILevelTextureStorage::Source>()(*level);
                        auto mesh_storage = injector.create<IMeshStorage::Source>()(*level, *texture_storage);
                        return std::make_unique<Level>(
                            injector.create<std::shared_ptr<IDevice>>(),
                            injector.create<std::shared_ptr<IShaderStorage>>(),
                            std::move(level),
                            texture_storage,
                            std::move(mesh_storage),
                            injector.create<std::unique_ptr<ITransparencyBuffer>>(),
                            injector.create<std::unique_ptr<ISelectionRenderer>>(),
                            injector.create<std::shared_ptr<ITypeNameLookup>>(),
                            injector.create<IEntity::EntitySource>(),
                            injector.create<IEntity::AiSource>(),
                            injector.create<IRoom::Source>(),
                            injector.create<ITrigger::Source>(),
                            injector.create<ILight::Source>());
                    };
                })
        );
    }
}
