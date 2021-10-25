#pragma once

#include "IMover.h"

namespace trview
{
    class Mover final : public IMover
    {
    public:
        virtual ~Mover() = default;
        virtual void render(const ICamera& camera) override;
    };
}
