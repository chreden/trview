export module trview.app:IPicking;

import trview.common;

import :PickInfo;
import :PickResult;

namespace trview
{
    struct ICamera;

    export struct IPicking
    {
        virtual ~IPicking() = 0;

        virtual void pick(const ICamera& camera) = 0;

        /// The sources of pick information.
        Event<PickInfo, PickResult&> pick_sources;

        /// Raise when something has been picked.
        Event<PickInfo, PickResult> on_pick;
    };
}
