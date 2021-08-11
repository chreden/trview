#pragma once

#include <optional>
#include <set>

#include <Windows.h>
#include <shellapi.h>
#include <ShlObj.h>

#include <trview.common/TokenStore.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>
#include <trview.tests.common/Window.h>
#include <trview.tests.common/Mocks.h>

namespace std
{
    namespace detail
    {
        template<class T>
        struct invoke_impl {
            template<class F, class... Args>
            static auto call(F&& f, Args&&... args)
                -> decltype(std::forward<F>(f)(std::forward<Args>(args)...));
        };

        template<class F, class... Args, class Fd = typename std::decay<F>::type>
        auto INVOKE(F&& f, Args&&... args)
            -> decltype(invoke_impl<Fd>::call(std::forward<F>(f), std::forward<Args>(args)...));
    }

    // Minimal C++11 implementation:
    template <class> struct result_of;
    template <class F, class... ArgTypes>
    struct result_of<F(ArgTypes...)> {
        using type = decltype(detail::INVOKE(std::declval<F>(), std::declval<ArgTypes>()...));
    };
}

#include "gtest/gtest.h"
#include "gmock/gmock.h"

// Fix for printing ComPtr - removes value printing:
namespace testing
{
    namespace internal
    {
        template <typename T>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<T>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<T>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };
    }
}

