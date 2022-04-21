#pragma once

#include "IPicking.h"

namespace trview
{
    class Picking final : public IPicking
    {
    public:
        virtual ~Picking() = default;

        explicit Picking(const Window& window);

        /// <summary>
        /// Perform a pick operation.
        /// </summary>
        /// <param name="camera">The current scene camera.</param>
        virtual void pick(const ICamera& camera) override;
    private:
        Window _window;
    };
}
