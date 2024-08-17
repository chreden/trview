#pragma once

#include <vector>
#include <memory>

#include <trlevel/LevelVersion.h>

namespace trview
{
    struct ISoundSource;
    struct ISoundsWindow;
    struct ISoundStorage;
    struct ISoundsWindowManager
    {
        virtual ~ISoundsWindowManager() = 0;
        virtual std::weak_ptr<ISoundsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_level_version(trlevel::LevelVersion version) = 0;
        virtual void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) = 0;
        virtual void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) = 0;
    };
}
