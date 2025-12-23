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
        virtual std::weak_ptr<IStaticsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void update(float dt) = 0;
    };
}