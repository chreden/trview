#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <trview.common/Event.h>

namespace trview
{
    struct ILevel;
    struct ISoundSource;
    struct ISoundStorage;
    struct ISoundsWindow
    {
        using Source = std::function<std::shared_ptr<ISoundsWindow>()>;

        virtual ~ISoundsWindow() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source) = 0;

        Event<> on_scene_changed;
        Event<> on_window_closed;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
    };
}
