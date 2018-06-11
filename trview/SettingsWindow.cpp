#include "stdafx.h"
#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/StackPanel.h>
#include <trview.ui/GroupBox.h>

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent)
    {
        const auto background_colour = Colour(1.0f, 0.4f, 0.4f, 0.4f);

        auto window = std::make_unique<StackPanel>(Point(400, 200), Size(200, 100), background_colour, Size(5,5));

        // Things to add:
        //      Window title
        //      Close button
        //      Checkboxes

        auto group = std::make_unique<GroupBox>(Point(), Size(190, 90), background_colour, Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Settings");

        window->add_child(std::move(group));

        parent.add_child(std::move(window));
    }
}
