module;

#include <gmock/gmock.h>

export module trview.app:MockMeasure;

import :IMeasure;

namespace trview
{
    namespace mocks
    {
        struct MockMeasure : public IMeasure
        {
            MockMeasure();
            virtual ~MockMeasure();
            MOCK_METHOD(void, reset, (), (override));
            MOCK_METHOD(bool, add, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, set, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, render, (const ICamera&), (override));
            MOCK_METHOD(std::string, distance, (), (const, override));
            MOCK_METHOD(bool, measuring, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
        };
    }
}
