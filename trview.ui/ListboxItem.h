#pragma once

#include <unordered_map>

namespace trview
{
    namespace ui
    {
        class ListboxItem final
        {
        public:

        private:
            std::unordered_map<std::string, std::string> _values;
        };
    }
}
