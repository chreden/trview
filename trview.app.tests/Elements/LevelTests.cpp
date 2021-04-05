#include <trview.app/Elements/Level.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.app/Mocks/Geometry/ITransparencyBuffer.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/ILevelTextureStorage.h>
#include <trview.app/Mocks/Graphics/IMeshStorage.h>
#include <trview.app/Mocks/Graphics/ISelectionRenderer.h>
#include <trview.app/Mocks/Elements/ITypeNameLookup.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using testing::NiceMock;
using testing::Return;

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

    auto mock_type_name_lookup = std::make_shared<MockTypeNameLookup>();
    EXPECT_CALL(*mock_type_name_lookup, lookup_type_name(LevelVersion::Tomb2, 123));
    Level level(std::make_shared<MockDevice>(), std::make_shared<MockShaderStorage>(), std::move(mock_level),
        std::make_unique<MockLevelTextureStorage>(), std::make_unique<MockMeshStorage>(), std::make_unique<MockTransparencyBuffer>(),
        std::make_unique<MockSelectionRenderer>(), mock_type_name_lookup, [](auto, auto, auto, auto, auto) {return std::make_unique<MockMesh>(); });
}