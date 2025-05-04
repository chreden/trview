#include <trview.app/Sound/SoundStorage.h>
#include <trview.app/Mocks/Sound/ISound.h>
#include <trview.tests.common/Mocks.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

TEST(SoundStorage, AddAndGetSound)
{
    auto sound = mock_shared<MockSound>();
    auto source = [&](auto&&...) { return sound; };
    SoundStorage storage(source);

    auto found = storage.get(100).lock();
    ASSERT_EQ(found, nullptr);

    storage.add({ .sound_map = 0, .sound_details = 0, .sample_index = 100 }, {});

    found = storage.get(100).lock();
    ASSERT_EQ(found, sound);
}

TEST(SoundStorage, AddAndGetSounds)
{
    auto sound = mock_shared<MockSound>();
    auto source = [&](auto&&...) { return sound; };
    SoundStorage storage(source);

    auto sounds = storage.sounds();
    ASSERT_EQ(sounds.empty(), true);

    storage.add({ .sound_map = 0, .sound_details = 0, .sample_index = 100 }, {});

    sounds = storage.sounds();
    ASSERT_EQ(sounds.size(), 1);
    ASSERT_EQ(sounds[0].index.sample_index, 100);
    ASSERT_EQ(sounds[0].sound.lock(), sound);
}
