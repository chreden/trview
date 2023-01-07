#pragma once

namespace trview
{
    enum class Type
    {
        CameraSink,
        Item,
        Light,
        Room,
        Trigger
    };

    inline constexpr std::string to_string(Type type) noexcept
    {
        switch (type)
        {
        case Type::CameraSink:
            return "Camera/Sink";
        case Type::Item:
            return "Item";
        case Type::Light:
            return "Light";
        case Type::Room:
            return "Room";
        case Type::Trigger:
            return "Trigger";
        }
        return "Unknown";
    }

    template <Type... Args>
    class Track
    {
    public:
        void render();
        bool enabled() const;
        bool enabled(Type type) const;
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
