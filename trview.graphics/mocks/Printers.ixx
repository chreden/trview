module;

#include <gtest/gtest.h>

export module trview.graphics:Printers;

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