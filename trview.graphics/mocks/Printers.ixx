module;

#include <gtest/gtest.h>
#include <d3d11.h>
#include <wrl/client.h>

export module trview.graphics:Printers;

namespace testing
{
    namespace internal
    {
        template <>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };

        template <>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<ID3D11Texture2D>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };

        template <>
        class UniversalPrinter<const Microsoft::WRL::ComPtr<ID3D11Resource>&> {
        public:
            // MSVC warns about adding const to a function type, so we want to
            // disable the warning.
            GTEST_DISABLE_MSC_WARNINGS_PUSH_(4180)

                static void Print(const Microsoft::WRL::ComPtr<ID3D11Resource>& value, ::std::ostream* os) {
                UniversalPrint(value, os);
            }

            GTEST_DISABLE_MSC_WARNINGS_POP_()
        };
    }
}