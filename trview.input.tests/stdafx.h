#pragma once

#include <utility>

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