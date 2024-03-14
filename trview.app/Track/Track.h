#pragma once

#include "../Type.h"
#include <array>
#include <trview.common/Event.h>

namespace trview
{
    template <Type... Args>
    class Track
    {
    public:
        struct Names
        {
            static const inline std::string Enable = "##TrackEnable";
            static const inline std::string Button = "Track##track";
            static const inline std::string Popup = "Track";
        };

        void render();
        bool enabled() const;
        template <Type T>
        bool enabled() const;
        template <Type T>
        Event<bool>& on_toggle();
        template <Type T>
        void set_enabled(bool value);
    private:
        void toggle_visible();

        struct Subject
        {
            Type type;
            Event<bool> on_toggle;
            bool value{ false };
        };

        std::array<Subject, sizeof...(Args)> state{ Subject{ Args, {}, false }... };
        bool _show{ false };
        bool _enabled{ true };
    };
}

#include "Track.inl"
