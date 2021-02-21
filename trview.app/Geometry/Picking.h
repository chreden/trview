#pragma once

#include "IPicking.h"

namespace trview
{
    class Picking final : public IPicking
    {
    public:
        virtual ~Picking() = default;

        /// Perform a pick operation.
        /// @param window The window that the scene is being rendered in.
        /// @param camera The current scene camera.
        virtual void pick(const Window& window, const ICamera& camera) override;
    };
}
