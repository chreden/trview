#pragma once

#include "../../Tools/IMeasure.h"

namespace trview
{
    class MockMeasure final : public IMeasure
    {
    public:
        virtual ~MockMeasure() = default;
        MOCK_METHOD(void, reset, ());
        MOCK_METHOD(bool, add, (const DirectX::SimpleMath::Vector3&));
        MOCK_METHOD(void, set, (const DirectX::SimpleMath::Vector3&));
        MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&));
        MOCK_METHOD(std::wstring, distance, (), (const));
        MOCK_METHOD(bool, measuring, (), (const));
        MOCK_METHOD(void, set_visible, (bool));
    };
}
