#include <windows.h>
#undef Yield

#include "RoomNavigatorTests.h"
#include <imgui_te_context.h>
#include <imgui_te_engine.h>
#include <imgui_te_internal.h>
#include <format>
#include <optional>

#include <trview.app/UI/RoomNavigator.h>

#include "TestHelpers.h"

using namespace trview;

void register_room_navigator_controls_tests(ImGuiTestEngine* engine)
{
    test<RoomNavigator>(engine, "Room Navigator", "Enter Room Number",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();
            navigator.set_max_rooms(10);

            std::optional<uint32_t> raised;
            auto token = navigator.on_room_selected += [&](auto room)
            {
                raised = room;
            };

            ctx->SetRef("Room Navigator");
            ctx->ItemInputValue("of 9##roomnumber", "5");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), 5);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Max Rooms Limits Current Room",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();
            navigator.set_max_rooms(0);

            std::optional<uint32_t> raised;
            auto token = navigator.on_room_selected += [&](auto room)
            {
                raised = room;
            };

            ctx->SetRef("Room Navigator");
            ctx->ItemClick("of 0##roomnumber/+");

            IM_CHECK_EQ(raised.has_value(), false);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Max Rooms Updates Label",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();
            navigator.set_max_rooms(10);

            ctx->SetRef("Room Navigator");
            IM_CHECK_EQ(ctx->ItemExists("of 9##roomnumber"), true);

            navigator.set_max_rooms(100);
            ctx->Yield();
            IM_CHECK_EQ(ctx->ItemExists("of 99##roomnumber"), true);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Min Rooms Limits Current Room",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();
            navigator.set_max_rooms(10);

            std::optional<uint32_t> raised;
            auto token = navigator.on_room_selected += [&](auto room)
            {
                raised = room;
            };

            ctx->SetRef("Room Navigator");
            ctx->ItemClick("of 9##roomnumber/-");

            IM_CHECK_EQ(raised.has_value(), false);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Room Selected Event Raised On Minus",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();

            std::optional<uint32_t> raised;
            auto token = navigator.on_room_selected += [&](auto room)
            {
                raised = room;
            };

            navigator.set_selected_room(5);
            navigator.set_max_rooms(10);

            ctx->SetRef("Room Navigator");
            ctx->ItemClick("of 9##roomnumber/-");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), 4);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Room Selected Event Raised On Minus",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();

            std::optional<uint32_t> raised;
            auto token = navigator.on_room_selected += [&](auto room)
            {
                raised = room;
            };

            navigator.set_max_rooms(10);

            ctx->SetRef("Room Navigator");
            ctx->ItemClick("of 9##roomnumber/+");

            IM_CHECK_EQ(raised.has_value(), true);
            IM_CHECK_EQ(raised.value(), 1);
        });

    test<RoomNavigator>(engine, "Room Navigator", "Set Current Room Updates Up/Down",
        [](ImGuiTestContext* ctx) { ctx->GetVars<RoomNavigator>().render(); },
        [](ImGuiTestContext* ctx)
        {
            auto& navigator = ctx->GetVars<RoomNavigator>();
            navigator.set_max_rooms(10);

            ctx->SetRef("Room Navigator");
            auto id = ctx->ItemInfo("of 9##roomnumber")->ID;
            IM_CHECK_EQ(ItemText(ctx, id), "0");

            navigator.set_selected_room(1);
            ctx->Yield();

            IM_CHECK_EQ(ItemText(ctx, id), "1");
        });
}
