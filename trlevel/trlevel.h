#pragma once

#include <memory>
#include <string>

#include "ILevel.h"

namespace trlevel
{
    // Load the level at the specified location.
    // Returns: The loaded level.
    std::unique_ptr<ILevel> load_level(const std::string& filename);
}