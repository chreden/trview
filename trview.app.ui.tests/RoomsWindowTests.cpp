#include "pch.h"
#include <trview.app/Windows/RoomsWindow.h>
#include <trview.app/Mocks/UI/IMapRenderer.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return mock_unique<MockMapRenderer>(); } };
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<RoomsWindow> build()
            {
                return std::make_unique<RoomsWindow>(map_renderer_source, clipboard, messaging);
            }

            test_module& with_map_renderer_source(IMapRenderer::Source map_renderer_source)
            {
                this->map_renderer_source = map_renderer_source;
                return *this;
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }
        };
        return test_module{};
    }

    struct RoomsWindowContext final
    {
        std::shared_ptr<RoomsWindow> ptr;
        std::vector<std::shared_ptr<IRoom>> rooms;

        void render()
        {
            if (ptr)
            {
                ptr->render();
            }
        }
    };
}

void register_rooms_window_tests(ImGuiTestEngine* engine)
{
    test<RoomsWindowContext>(engine, "Rooms Window", "Level Version Changes Flags",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RoomsWindowContext>();
            context.ptr = register_test_module().build();

            auto room = mock_shared<MockRoom>();
            EXPECT_CALL(*room, flag).Times(testing::AtLeast(1)).WillRepeatedly(testing::Return(true));
            context.rooms.push_back(room);

            context.ptr->set_level_version(trlevel::LevelVersion::Tomb1);
            context.ptr->set_rooms({ room });
            context.ptr->set_current_room(room);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/Bit 7"), true);

            auto props = ctx->ItemInfo("/**/Bit 7");
            ctx->SetRef(props->Window);

            IM_CHECK_EQ(ctx->ItemExists("Quicksand \\/ 7"), false);

            context.ptr->set_level_version(trlevel::LevelVersion::Tomb3);
            context.ptr->set_rooms({ room });
            context.ptr->set_current_room(room);
            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/Bit 7"), false);
            ctx->SetRef(props->Window);
            IM_CHECK_EQ(ctx->ItemExists("Quicksand \\/ 7"), false);
        });

    test<RoomsWindowContext>(engine, "Rooms Window", "On Room Visibility Raised",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RoomsWindowContext>();
            context.ptr = register_test_module().build();

            auto room1 = mock_shared<MockRoom>()->with_number(0);
            auto room2 = mock_shared<MockRoom>()->with_number(1)->with_updating_visible(false);
            EXPECT_CALL(*room2, set_visible(true)).Times(1);
            context.ptr->set_rooms({ room1, room2 });

            ctx->ItemUncheck("/**/##Hide-1");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(room2.get()), true);
        });

    test<RoomsWindowContext>(engine, "Rooms Window", "Floordata Type Filters List",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomsWindowContext>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<RoomsWindowContext>();
            context.ptr = register_test_module().build();
            context.ptr->set_floordata({ 0x000, 0x8005 });

            auto normal_room = mock_shared<MockRoom>()->with_number(0);
            auto death_room = mock_shared<MockRoom>()->with_number(1);
            context.rooms = { normal_room, death_room };

            auto normal_sector = mock_shared<MockSector>();
            auto death_sector = mock_shared<MockSector>();
            ON_CALL(*normal_sector, floordata_index).WillByDefault(Return(0));
            ON_CALL(*death_sector, floordata_index).WillByDefault(Return(1));

            ON_CALL(*normal_room, sectors).WillByDefault(Return(std::vector<std::shared_ptr<ISector>>{ normal_sector }));
            ON_CALL(*death_room, sectors).WillByDefault(Return(std::vector<std::shared_ptr<ISector>>{ death_sector }));

            context.ptr->set_level_version(trlevel::LevelVersion::Tomb1);
            context.ptr->set_rooms({ normal_room, death_room });
            context.ptr->set_current_room(normal_room);

            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), true);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);

            ctx->ItemClick("/**/Filters##FiltersButton");
            ctx->SetRef(ctx->ItemInfo("/**/+")->Window);
            ctx->ItemClick("+");

            ctx->ComboClick("##filter-key-0/Floordata Type");
            ctx->ComboClick("##filter-compare-op-0/is");
            ctx->ComboClick("##filter-value-0/Death");

            ctx->Yield();

            IM_CHECK_EQ(ctx->ItemExists("/**/##0"), false);
            IM_CHECK_EQ(ctx->ItemExists("/**/##1"), true);
        });
}
