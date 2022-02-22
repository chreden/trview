#include <trview.app/UI/LevelInfo.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using testing::Return;

TEST(LevelInfo, NameUpdated)
{
    MockTextureStorage texture_storage;
    LevelInfo info(texture_storage);

    TestImgui imgui([&]() { info.render(); });
    info.set_level("test");
    imgui.render();

    auto rendered = imgui.rendered_text(imgui.id("LevelInfo").id(""));
    ASSERT_THAT(rendered, testing::Contains("test"));
}

TEST(LevelInfo, OnToggleSettingsRaised)
{
    MockTextureStorage texture_storage;
    LevelInfo info(texture_storage);
    TestImgui imgui([&]() { info.render(); });

    bool raised = false;
    auto token = info.on_toggle_settings += [&]()
    {
        raised = true;
    };

    imgui.click_element(imgui.id("LevelInfo").id("Settings"));
    ASSERT_TRUE(raised);
}
