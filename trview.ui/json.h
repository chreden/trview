#pragma once

#include "Control.h"

namespace trview
{
    namespace ui
    {
        std::unique_ptr<Control> load_from_json(const std::string& json);
    }
}