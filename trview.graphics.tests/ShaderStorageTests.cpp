#include "gtest/gtest.h"
#include <trview.graphics/ShaderStorage.h>

using namespace trview::graphics;

class TestShader final : public IShader
{
public:
    virtual ~TestShader() = default;
    virtual void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>&) override {}
};

/// Test that the shader is added to the store and can then be retrieved.
TEST(ShaderStorage, AddAndRetrieveShader)
{
    ShaderStorage storage;
    ASSERT_EQ(nullptr, storage.get("test"));
    storage.add("test", std::make_unique<TestShader>());
    ASSERT_NE(nullptr, storage.get("test"));
}

/// Test that a null shader is rejected by the store.
TEST(ShaderStorage, NullShader)
{
    ShaderStorage storage;
    ASSERT_EQ(nullptr, storage.get("test"));
    ASSERT_THROW(storage.add("test", nullptr), std::exception);
    ASSERT_EQ(nullptr, storage.get("test"));
}
