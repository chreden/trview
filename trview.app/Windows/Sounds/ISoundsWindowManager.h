#pragma once

#include <vector>
#include <memory>

#include <trlevel/LevelVersion.h>

namespace trview
{
    struct UserSettings;
    struct ISoundSource;
    struct ISoundsWindow;
    struct ISoundStorage;
    struct ISoundsWindowManager
    {
        virtual ~ISoundsWindowManager() = 0;
        virtual std::weak_ptr<ISoundsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void select_sound_source(const std::weak_ptr<ISoundSource>& sound_source) = 0;
        virtual void set_level_platform(trlevel::Platform platform) = 0;
        virtual void set_level_version(trlevel::LevelVersion version) = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
        virtual void set_sound_sources(const std::vector<std::weak_ptr<ISoundSource>>& sound_sources) = 0;
        virtual void set_sound_storage(const std::weak_ptr<ISoundStorage>& sound_storage) = 0;

        Event<UserSettings> on_settings;
        Event<std::weak_ptr<ISoundSource>> on_sound_source_selected;
    };
}
