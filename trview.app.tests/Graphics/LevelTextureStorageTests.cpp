#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Graphics/LevelTextureStorage.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>
#include <ranges>

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
using testing::SaveArg;
using namespace trview::graphics::mocks;

TEST(LevelTextureStorage, PaletteLoadedTomb1)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, platform_and_version()).WillRepeatedly(Return(trlevel::PlatformAndVersion{ .version = LevelVersion::Tomb1 }));
    EXPECT_CALL(*level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>());
    subject.load(level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb2)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, platform_and_version()).WillRepeatedly(Return(trlevel::PlatformAndVersion{ .version = LevelVersion::Tomb2 }));
    EXPECT_CALL(*level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>());
    subject.load(level);
}

TEST(LevelTextureStorage, PaletteLoadedTomb3)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, platform_and_version()).WillRepeatedly(Return(trlevel::PlatformAndVersion{ .version = LevelVersion::Tomb3 }));
    EXPECT_CALL(*level, get_palette_entry(_)).Times(AtLeast(1));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>());
    subject.load(level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb4)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, platform_and_version()).WillRepeatedly(Return(trlevel::PlatformAndVersion{ .version = LevelVersion::Tomb4 }));
    EXPECT_CALL(*level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>());
    subject.load(level);
}

TEST(LevelTextureStorage, PaletteNotLoadedTomb5)
{
    auto level = mock_shared<trlevel::mocks::MockLevel>();
    EXPECT_CALL(*level, platform_and_version()).WillRepeatedly(Return(trlevel::PlatformAndVersion{ .version = LevelVersion::Tomb5 }));
    EXPECT_CALL(*level, get_palette_entry(_)).Times(Exactly(0));
    LevelTextureStorage subject(mock_shared<MockDevice>(), mock_unique<MockTextureStorage>());
    subject.load(level);
}

TEST(LevelTextureStorage, TexturesGenerated)
{
    std::vector<uint32_t> data;
    data.resize(256 * 256, 0x000080ff);

    auto device = mock_shared<MockDevice>();

    std::vector<std::vector<uint32_t>> saved_data;
    EXPECT_CALL(*device, create_texture_2D).Times(1).WillRepeatedly(testing::Invoke(
        [&](auto&& desc, auto&& data)
        {
            const uint32_t* ptr = reinterpret_cast<const uint32_t*>(data.pSysMem);
            saved_data.push_back({ ptr, ptr + 256 * 256 });
            return nullptr;
        }));
    EXPECT_CALL(*device, create_shader_resource_view).Times(1);

    LevelTextureStorage subject(device, mock_unique<MockTextureStorage>());

    // 0: TRLE
    ASSERT_EQ(saved_data.size(), 1u);
    ASSERT_EQ(saved_data[0].size(), 65536u);
}
