module;

#include <gmock/gmock.h>

export module trview.graphics:MockSamplerState;

import :ISamplerState;

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            export struct MockSamplerState : public ISamplerState
            {
                MockSamplerState();
                virtual ~MockSamplerState();
                MOCK_METHOD(void, apply, (), (override));
                MOCK_METHOD(void, set_filter_mode, (FilterMode), (override));
            };
        }
    }
}
