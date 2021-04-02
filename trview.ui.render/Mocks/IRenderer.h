#pragma once

#include "../IRenderer.h"

namespace trview
{
    namespace ui
    {
        namespace render
        {
            namespace mocks
            {
                class MockRenderer : public IRenderer
                {
                public:
                    virtual ~MockRenderer() = default;
                    MOCK_METHOD(void, load, (Control*));
                    MOCK_METHOD(void, render, ());
                    MOCK_METHOD(void, set_host_size, (const Size&));
                };
            }
        }
    }
}