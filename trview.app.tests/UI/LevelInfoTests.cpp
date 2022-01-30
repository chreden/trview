#include <trview.app/UI/LevelInfo.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.app/Mocks/Graphics/ITextureStorage.h>

using namespace trview;
using namespace trview::mocks;
using testing::Return;

/*
TEST(LevelInfo, NameUpdated)
{
    ui::Window parent(Size(), Colour::White);
    MockTextureStorage texture_storage;
    LevelInfo info(parent, texture_storage, JsonLoader(std::make_shared<MockShell>()));

    info.set_level("test");

    auto label = parent.find<Label>(LevelInfo::Names::name);
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->text(), L"test");
}

TEST(LevelInfo, OnToggleSettingsRaised)
{
    ui::Window parent(Size(), Colour::White);
    MockTextureStorage texture_storage;
    LevelInfo info(parent, texture_storage, JsonLoader(std::make_shared<MockShell>()));

    bool raised = false;
    auto token = info.on_toggle_settings += [&]()
    {
        raised = true;
    };

    auto button = parent.find<Button>(LevelInfo::Names::settings);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(raised);
}

TEST(LevelInfo, VersionImageChanged)
{
    ui::Window parent(Size(), Colour::White);
    MockTextureStorage texture_storage;
    graphics::Texture texture;
    texture.set_name("TR3");
    ON_CALL(texture_storage, lookup("tomb3_icon")).WillByDefault(Return(texture));

    LevelInfo info(parent, texture_storage, JsonLoader(std::make_shared<MockShell>()));

    info.set_level_version(trlevel::LevelVersion::Tomb3);

    auto image = parent.find<Image>(LevelInfo::Names::version);
    ASSERT_NE(image, nullptr);
    ASSERT_EQ(image->texture().name(), "TR3");
}

TEST(LevelInfo, CentresWhenParentResizes)
{
    ui::Window parent(Size(400, 400), Colour::White);
    MockTextureStorage texture_storage;
    LevelInfo info(parent, texture_storage, JsonLoader(std::make_shared<MockShell>()));

    auto window = parent.child_elements()[0];
    ASSERT_NE(window, nullptr);
    ASSERT_EQ(window->position().x, parent.size().width / 2 - window->size().width / 2);

    parent.set_size(Size(800, 400));
    ASSERT_EQ(window->position().x, parent.size().width / 2 - window->size().width / 2);
}
*/