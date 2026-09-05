module;

#include <gmock/gmock.h>

export module trview.graphics:MockShaderStorage;

import :IShaderStorage;

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            export class MockShaderStorage : public IShaderStorage
            {
            public:
                MOCK_METHOD(void, add, (const std::string&, std::unique_ptr<IShader>));
                MOCK_METHOD(IShader*, get, (const std::string&), (const));
            };
        }
    }
}
