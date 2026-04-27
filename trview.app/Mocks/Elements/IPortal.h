#pragma once

#include "../../Elements/Portal/IPortal.h"

namespace trview
{
    namespace mocks
    {
        struct MockPortal : public IPortal
        {
            explicit MockPortal();
            virtual ~MockPortal();
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, normal, (), (const, override));
            MOCK_METHOD(uint16_t, room, (), (const, override));
            MOCK_METHOD(std::vector<DirectX::SimpleMath::Vector3>, vertices, (), (const, override));
        };
    }
}
