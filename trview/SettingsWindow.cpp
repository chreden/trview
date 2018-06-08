#include "stdafx.h"
#include "SettingsWindow.h"

#include <trview.ui/Control.h>
#include <trview.ui/Window.h>

using namespace trview::ui;

namespace trview
{
    SettingsWindow::SettingsWindow(Control& parent)
    {
        auto window = std::make_unique<Window>(Point(400, 200), Size(200, 100), Colour(1.0f, 0.2f, 0.2f, 0.2f));
        parent.add_child(std::move(window));
    }
}
