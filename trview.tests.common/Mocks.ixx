module;

#include <gtest/gtest.h>
#include <gmock/gmock.h>

export module trview.tests.common:Mocks;

import std;

namespace trview
{
    namespace tests
    {
        /// <summary>
        /// Create a mock in a unique ptr. May add additional wrappers.
        /// </summary>
        /// <typeparam name="T">The mock type.</typeparam>
        /// <returns>Mock pointer and reference to mock.</returns>
        export template <typename T>
        std::tuple<std::unique_ptr<T>, T&> create_mock();

        /// <summary>
        /// Create a mock in a unique_ptr. May add additional wrappers.
        /// </summary>
        /// <typeparam name="T">The mock type.</typeparam>
        /// <returns>Mock pointer</returns>
        export template <typename T>
        auto mock_unique();

        /// <summary>
        /// Create a mock in a shared_ptr. May add additional wrappers.
        /// </summary>
        /// <typeparam name="T">The mock type.</typeparam>
        /// <returns>The new mock.</returns>
        export template <typename T>
        auto mock_shared();
    }
}

#include "Mocks.hpp"