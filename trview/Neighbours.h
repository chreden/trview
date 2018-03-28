#pragma once

#include <trview.common/Event.h>
#include <cstdint>

namespace trview
{
    namespace ui
    {
        class Control;
        class Checkbox;
    }

    struct ITextureStorage;

    // The neighbours control has settings for whether neighbour mode is enabled and to allow
    // the user to select the depth of neighbours to display.
    class Neighbours
    {
    public:
        Neighbours(ui::Control& control, const ITextureStorage& texture_storage);

        // Event raised when the user has enabled or disabled neighbour mode.
        // enabled: Whether neighbours mode is enabled.
        Event<bool> on_enabled_changed;

        // Event raised when the user has changed the depth of neighbour to display.
        // depth: The new depth.
        Event<uint32_t> on_depth_changed;

        // Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
        // value: Whether neighbours are enabled.
        void set_enabled(bool value);
    private:
        ui::Checkbox* _enabled;
    };
}
