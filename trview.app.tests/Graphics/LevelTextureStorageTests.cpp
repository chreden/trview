#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Graphics/LevelTextureStorage.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>

using namespace trview;
using namespace trlevel;
using namespace trlevel::mocks;
using namespace trview::mocks;
using namespace trview::tests;
using testing::Return;
using testing::_;
using testing::AtLeast;
using testing::Exactly;
using testing::NiceMock;
using namespace trview::graphics::mocks;

TEST(LevelTextureStorage, PaletteLoadedTomb1)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb1));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb2)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb3)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb3));
    EXPECT_CALL(level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb4)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb4));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>(), level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb5)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    EXPECT_CALL(level, get_version()).WillRepeatedly(Return(LevelVersion::Tomb5));
    EXPECT_CALL(level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>(), level);
}
