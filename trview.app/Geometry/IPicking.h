#pragma once

#include <trview.common/Event.h>

#include "PickInfo.h"
#include "PickResult.h"

namespace trview
{
    struct ICamera;

    struct IPicking
    {
        virtual ~IPicking() = 0;

        virtual void pick(const ICamera& camera) = 0;

        /// The sources of pick information.
        Event<PickInfo, PickResult&> pick_sources;

        /// Raise when something has been picked.
        Event<PickInfo, PickResult> on_pick;
    };
}
