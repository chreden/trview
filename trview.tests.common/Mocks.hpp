#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace trview
{
    namespace tests
    {
        template <typename T>
        std::tuple<std::unique_ptr<T>, T&> create_mock()
        {
            auto ptr = std::make_unique<testing::NiceMock<T>>();
            auto& ref = *ptr;
            return { std::move(ptr), ref };
        }

        template <typename T>
        auto mock_unique()
        {
            return std::make_unique<testing::NiceMock<T>>();;
        }

        template <typename T>
        auto mock_shared()
        {
            return std::make_shared<testing::NiceMock<T>>();
        }

        template <typename Mock, typename T>
        auto choose_mock(std::unique_ptr<T>& ptr)
        {
            if (!ptr)
            {
                ptr = std::make_unique<Mock>();
            }
        }

        template <typename Mock, typename T>
        auto choose_mock(std::shared_ptr<T>& ptr)
        {
            if (!ptr)
            {
                ptr = mock_shared<Mock>();
            }
        }
    }
}