#include <trview.app/Windows/LightsWindow.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<LightsWindow> build()
            {
                return std::make_unique<LightsWindow>(clipboard);
            }
        };

        return test_module{};
    }
}

TEST(LightsWindow, LightSelectedRaisedWhenSyncItemEnabled)
{
    auto window = register_test_module().build();

    std::weak_ptr<ILight> raised_light;
    auto token = window->on_light_selected += [&raised_light](const auto& light) { raised_light = light; };

    auto light1 = mock_shared<MockLight>()->with_number(0);
    auto light2 = mock_shared<MockLight>()->with_number(1);
    window->set_lights({ light1, light2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("1##1"));

    ASSERT_EQ(raised_light.lock(), light2);
}

TEST(LightsWindow, LightSelectedNotRaisedWhenSyncItemDisabled)
{
    auto window = register_test_module().build();

    std::weak_ptr<ILight> raised_light;
    auto token = window->on_light_selected += [&raised_light](const auto& light) { raised_light = light; };

    auto light1 = mock_shared<MockLight>()->with_number(0);
    auto light2 = mock_shared<MockLight>()->with_number(1);
    window->set_lights({ light1, light2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .id(LightsWindow::Names::sync_light));
    imgui.click_element_with_hover(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("1##1"));

    ASSERT_EQ(raised_light.lock(), nullptr);
}

TEST(LightsWindow, OnLightVisibilityRaised)
{
    auto window = register_test_module().build();

    std::tuple<std::weak_ptr<ILight>, bool> raised_light;
    auto token = window->on_light_visibility += [&raised_light](const auto& light, auto&& visible)
    {
        raised_light = { light, visible };
    };

    auto light1 = mock_shared<MockLight>()->with_number(0);
    auto light2 = mock_shared<MockLight>()->with_number(1);
    window->set_lights({ light1, light2 });

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element_with_hover(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("##hide-1"));

    ASSERT_EQ(std::get<0>(raised_light).lock(), light2);
    ASSERT_TRUE(std::get<1>(raised_light));
}

TEST(LightsWindow, LightListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_room(56);
    auto light2 = mock_shared<MockLight>()->with_number(1)->with_room(78);
    window->set_lights({ light1, light2 });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    imgui.click_element(imgui.id("Lights 0")
        .push_child(LightsWindow ::Names::light_list_panel)
        .id(LightsWindow::Names::track_room));

    imgui.reset();
    imgui.render();

    ASSERT_FALSE(imgui.element_present(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("1##1")));
}

TEST(LightsWindow, LightsListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_room(56);
    auto light2 = mock_shared<MockLight>()->with_number(1)->with_room(78);
    window->set_lights({ light1, light2 });
    window->set_current_room(78);

    TestImgui imgui([&]() { window->render(); });
    ASSERT_TRUE(imgui.element_present(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("0##0")));
    ASSERT_TRUE(imgui.element_present(imgui.id("Lights 0")
        .push_child(LightsWindow::Names::light_list_panel)
        .push(LightsWindow::Names::lights_listbox).id("1##1")));
}

TEST(LightsWindow, PointLightStatsTR1)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb1);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_FALSE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Fade")));
    ASSERT_FALSE(imgui.element_present(id.id("Hotspot")));
    ASSERT_FALSE(imgui.element_present(id.id("Falloff")));
}

TEST(LightsWindow, PointLightStatsTR2)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb2);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_FALSE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Fade")));
    ASSERT_FALSE(imgui.element_present(id.id("Hotspot")));
    ASSERT_FALSE(imgui.element_present(id.id("Falloff")));
}

TEST(LightsWindow, PointLightStatsShownTR3)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb3);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_TRUE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Fade")));
    ASSERT_FALSE(imgui.element_present(id.id("Hotspot")));
    ASSERT_FALSE(imgui.element_present(id.id("Falloff")));
}

TEST(LightsWindow, PointLightStatsShownTR4)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Point);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb4);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_TRUE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_FALSE(imgui.element_present(id.id("Fade")));
    ASSERT_TRUE(imgui.element_present(id.id("Hotspot")));
    ASSERT_TRUE(imgui.element_present(id.id("Falloff")));
}

TEST(LightsWindow, SpotLightStatsShown)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Spot);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb4);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_TRUE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Direction")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Hotspot")));
    ASSERT_TRUE(imgui.element_present(id.id("Falloff Angle")));
    ASSERT_TRUE(imgui.element_present(id.id("Length")));
    ASSERT_TRUE(imgui.element_present(id.id("Cutoff")));
}

TEST(LightsWindow, SunLightStatsShown)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Sun);
    window->set_lights({ light1 });
    window->set_selected_light(light1);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_FALSE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Direction")));
}

TEST(LightsWindow, FogBulbStatsShown)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::FogBulb);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb4);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Density")));
    ASSERT_TRUE(imgui.element_present(id.id("Radius")));
}

TEST(LightsWindow, ShadowStatsShown)
{
    auto window = register_test_module().build();
    auto light1 = mock_shared<MockLight>()->with_number(0)->with_type(trlevel::LightType::Shadow);
    window->set_lights({ light1 });
    window->set_selected_light(light1);
    window->set_level_version(trlevel::LevelVersion::Tomb4);

    TestImgui imgui([&]() { window->render(); });

    auto id = imgui.id("Lights 0")
        .push_child(LightsWindow::Names::details_panel)
        .push(LightsWindow::Names::stats_listbox);

    ASSERT_TRUE(imgui.element_present(id.id("Type")));
    ASSERT_TRUE(imgui.element_present(id.id("#")));
    ASSERT_TRUE(imgui.element_present(id.id("Room")));
    ASSERT_TRUE(imgui.element_present(id.id("Colour")));
    ASSERT_TRUE(imgui.element_present(id.id("Position")));
    ASSERT_TRUE(imgui.element_present(id.id("Intensity")));
    ASSERT_TRUE(imgui.element_present(id.id("Hotspot")));
    ASSERT_TRUE(imgui.element_present(id.id("Falloff")));
}
