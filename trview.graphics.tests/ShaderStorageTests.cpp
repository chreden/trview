#include "CppUnitTest.h"

#include <trview.graphics/ShaderStorage.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace graphics
    {
        namespace tests
        {
            TEST_CLASS(ShaderStorageTests)
            {
            private:
                class TestShader final : public IShader
                {
                public:
                    virtual ~TestShader() = default;

                    virtual void apply(const CComPtr<ID3D11DeviceContext>& context) override
                    {
                    }
                };

            public:
                // Test that the shader is added to the store and can then be retrieved.
                TEST_METHOD(AddAndRetrieveShader)
                {
                    ShaderStorage storage;
                    Assert::IsNull(storage.get("test"));
                    storage.add("test", std::make_unique<TestShader>());
                    Assert::IsNotNull(storage.get("test"));
                }

                // Test that a null shader is rejected by the store.
                TEST_METHOD(NullShader)
                {
                    ShaderStorage storage;
                    Assert::IsNull(storage.get("test"));
                    Assert::ExpectException<std::exception>([&storage]() { storage.add("test", nullptr); });
                    Assert::IsNull(storage.get("test"));
                }
            };
        }
    }
}
