#pragma once

#include <vector>
#include <memory>
#include "IStaticsWindow.h"
#include "../../Elements/IStaticMesh.h"

namespace trview
{
    struct IStaticsWindowManager
    {
        virtual ~IStaticsWindowManager() = 0;
        virtual void render() = 0;
        virtual std::weak_ptr<IStaticsWindow> create_window() = 0;
        virtual void update(float dt) = 0;
        virtual void set_statics(const std::vector<std::weak_ptr<IStaticMesh>>& statics) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void select_static(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;

        Event<std::weak_ptr<IStaticMesh>> on_static_selected;
        Event<std::weak_ptr<IStaticMesh>, bool> on_static_visibility;
    };
}