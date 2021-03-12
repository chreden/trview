#pragma once

#include <trview.graphics/IShaderStorage.h>

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            class MockShaderStorage : public IShaderStorage
            {
            public:
                virtual ~MockShaderStorage() = default;
                MOCK_METHOD(void, add, (const std::string&, std::unique_ptr<IShader>));
                MOCK_METHOD(IShader*, get, (const std::string&), (const));
            };
        }
    }
}
