#pragma once

#include <tuple>
#include <memory>

namespace trview
{
    namespace tests
    {
        template <typename T>
        std::tuple<std::unique_ptr<T>, T&> create_mock();
    }
}

#include "Mocks.hpp"