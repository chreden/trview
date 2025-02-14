#pragma once

#include <vector>
#include <memory>

#include <trlevel/LevelVersion.h>

namespace trview
{
    struct ILevel;
    struct ISoundSource;
    struct ISoundsWindow;
    struct ISoundStorage;
    struct ISoundsWindowManager
    {
        virtual ~ISoundsWindowManager() = 0;
        virtual std::weak_ptr<ISoundsWindow> create_window() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void render() = 0;
        virtual void select_sound_source(const std::weak_ptr<ISoundSource>& sound_source) = 0;

        Event<> on_scene_changed;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
    };
}
