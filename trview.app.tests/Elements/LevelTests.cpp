#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>

using namespace trview;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using testing::NiceMock;
using testing::Return;

namespace
{
    class MockTypeNameLookup : public ITypeNameLookup
    {
    public:
        MOCK_METHOD(std::wstring, lookup_type_name, (LevelVersion, uint32_t), (const, override));
    };
}

// Tests that the level class loads the type names with the correct level version.
TEST(Level, LoadTypeNames)
{
    tr2_entity entity;
    entity.Room = 0;
    entity.TypeID = 123;

    auto mock_level = std::make_unique<testing::NiceMock<MockLevel>>();
    EXPECT_CALL(*mock_level, get_version)
        .WillRepeatedly(Return(LevelVersion::Tomb2));
    EXPECT_CALL(*mock_level, num_rooms())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, num_entities())
        .WillRepeatedly(Return(1));
    EXPECT_CALL(*mock_level, get_entity(0))
        .WillRepeatedly(Return(entity));

    MockTypeNameLookup mock_type_name_lookup;
    EXPECT_CALL(mock_type_name_lookup, lookup_type_name(LevelVersion::Tomb2, 123));

    Level level(std::make_shared<MockDevice>(), std::make_shared<MockShaderStorage>(), std::move(mock_level), mock_type_name_lookup);
}