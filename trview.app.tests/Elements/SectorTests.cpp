#include <trview.app/Elements/Sector.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>

using namespace trview;
using namespace trview::mocks;
using namespace trlevel;
using namespace trlevel::mocks;
using testing::NiceMock;
using testing::Return;

TEST(Sector, HighNumberedPortal)
{
    NiceMock<trlevel::mocks::MockLevel> level;
    ON_CALL(level, num_floor_data).WillByDefault(Return(3));
    std::vector<uint16_t> floor_data{ 0x0000, 0x8001, 378 };
    EXPECT_CALL(level, get_floor_data_all).WillRepeatedly(Return(floor_data));

    tr3_room tr_room{};
    tr_room.num_x_sectors = 1;
    tr_room.num_z_sectors = 1;
    tr_room_sector sector { 1, 0xffff, 255, 0, 255, 0 };
    auto room = trview::tests::mock_shared<MockRoom>();

    Sector s(level, tr_room, sector, 0, room);

    ASSERT_EQ(s.portal(), 378);
}
