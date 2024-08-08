#pragma once

#include <trview.common/Event.h>

namespace trview
{
    namespace tests
    {
        template <int index = 0, typename T>
        inline auto capture(std::shared_ptr<T>& out)
        {
            return [&](auto... in) { out = std::get<index>(std::tie(in...)).lock(); };
        }

        template <int index = 0, typename T>
        inline auto capture(T& out)
        {
            return [&](auto... in) { out = std::get<index>(std::tie(in...)); };
        }

        inline auto capture_called(bool& out)
        {
            return [&](auto&&...) { out = true; };
        }
    }
}
