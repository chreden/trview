#include "json.h"
#include "Window.h"

namespace trview
{
    namespace ui
    {
        std::unique_ptr<Control> load_from_json(const std::string& json)
        {
            return std::make_unique<ui::Window>(Size(), Colour::White);
        }
    }
}
