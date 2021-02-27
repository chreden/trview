#pragma once

namespace trview
{
    namespace tests
    {
        template <typename T>
        std::tuple<std::unique_ptr<T>, T&> create_mock()
        {
            auto ptr = std::make_unique<T>();
            auto& ref = *ptr;
            return { std::move(ptr), ref };
        }
    }
}