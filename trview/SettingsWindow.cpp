#include "stdafx.h"
#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Label.h>

#include "ITextureStorage.h"

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent, const ITextureStorage& texture_storage)
    {
        const auto background_colour = Colour(1.0f, 0.5f, 0.5f, 0.5f);

        auto window = std::make_unique<StackPanel>(Point(400, 200), Size(400, 300), background_colour, Size(5,5));

        // Things to add:
        //      Window title
        //      Close button
        //      Checkboxes

        auto group = std::make_unique<GroupBox>(Point(), Size(390, 290), background_colour, Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Settings");
        auto panel = std::make_unique<StackPanel>(Point(12, 20), Size(366, 250), background_colour, Size());

        const auto check_off = texture_storage.lookup("check_off");
        const auto check_on = texture_storage.lookup("check_on");

        auto vsync = std::make_unique<Checkbox>(Point(12, 20), Size(16, 16), check_off, check_on, L"Vsync");
        panel->add_child(std::move(vsync));

        group->add_child(std::move(panel));
        window->add_child(std::move(group));

        parent.add_child(std::move(window));
    }
}
