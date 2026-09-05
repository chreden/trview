export module trview.tests.common:Event;

import trview.common;
import std;

namespace trview
{
    namespace tests
    {
        export template <int index = 0, typename T>
        inline auto capture(std::shared_ptr<T>& out)
        {
            return [&](auto... in) { out = std::get<index>(std::tie(in...)).lock(); };
        }

        export template <int index = 0, typename T>
        inline auto capture(T& out)
        {
            return [&](auto... in) { out = std::get<index>(std::tie(in...)); };
        }

        export inline auto capture_called(bool& out)
        {
            return [&](auto&&...) { out = true; };
        }
    }
}
