export module trview.app:ILevelInfo;

import std;

import trview.common;

import :ILevel;

namespace trview
{
    export struct ILevelInfo
    {
        virtual ~ILevelInfo() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;

        /// Event raised when the settings button is pressed.
        Event<> on_toggle_settings;
    };
}
