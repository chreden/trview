#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <trview.common/Event.h>

namespace trview
{
    struct ISoundSource;
    struct ISoundStorage;
    struct ISoundsWindow
    {
        using Source = std::function<std::shared_ptr<ISoundsWindow>()>;

        virtual ~ISoundsWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) = 0;
        virtual void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) = 0;

        Event<> on_window_closed;
    };
}
