#pragma once

#include "../Sampler/ISamplerState.h"

namespace trview
{
    namespace graphics
    {
        namespace mocks
        {
            struct MockSamplerState : public ISamplerState
            {
                MockSamplerState();
                virtual ~MockSamplerState();
                MOCK_METHOD(void, apply, (), (override));
                MOCK_METHOD(void, set_filter_mode, (FilterMode), (override));
            };
        }
    }
}
