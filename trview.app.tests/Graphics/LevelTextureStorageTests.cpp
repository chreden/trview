#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Graphics/LevelTextureStorage.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>

using namespace trview;
using namespace trlevel;
using namespace trlevel::mocks;
using namespace trview::mocks;
using testing::Return;
using testing::_;
using testing::AtLeast;
using testing::Exactly;
using namespace trview::graphics::mocks;

TEST(LevelTextureStorage, PaletteLoadedTomb1)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb1));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(std::make_shared<MockDevice>(), std::make_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb2)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(std::make_shared<MockDevice>(), std::make_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb3)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb3));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(std::make_shared<MockDevice>(), std::make_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb4)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(std::make_shared<MockDevice>(), std::make_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb5)
{
    MockLevel level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb5));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(std::make_shared<MockDevice>(), std::make_unique<MockTextureStorage>(), level);
}
