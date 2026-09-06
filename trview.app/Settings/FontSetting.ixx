export module trview.app:FontSetting;

import std;

namespace trview
{
    export struct FontSetting final
    {
        std::string name;
        std::string filename;
        int         size;
    };
}