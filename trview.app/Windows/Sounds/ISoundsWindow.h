#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <functional>

#include <trview.common/Event.h>
#include <trlevel/LevelVersion.h>

namespace trview
{
    struct ISoundSource;
    struct ISoundStorage;
    struct ISoundsWindow
    {
        using Source = std::function<std::shared_ptr<ISoundsWindow>()>;

        virtual ~ISoundsWindow() = 0;
        virtual void render() = 0;
        virtual void set_level_platform(trlevel::Platform platform) = 0;
        virtual void set_level_version(trlevel::LevelVersion version) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_sound_source(const std::weak_ptr<ISoundSource>& sound_source) = 0;
        virtual void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) = 0;
        virtual void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) = 0;

        Event<> on_window_closed;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
    };
}
