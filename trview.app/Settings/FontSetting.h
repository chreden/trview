#pragma once

#include <string>

namespace trview
{
    struct FontSetting final
    {
        std::string name;
        std::string filename;
        int         size;
    };
}