#pragma once

#include <trview.app/Elements/ILight.h>

namespace trview
{
    namespace mocks
    {
        struct MockLight : public ILight, public std::enable_shared_from_this<MockLight>
        {
            virtual ~MockLight() = default;
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(Colour, colour, (), (const, override));
            MOCK_METHOD(uint32_t, room, (), (const, override));
            MOCK_METHOD(trlevel::LightType, type, (), (const, override));
            MOCK_METHOD(int32_t, intensity, (), (const, override));
            MOCK_METHOD(int32_t, fade, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, direction, (), (const, override));
            MOCK_METHOD(float, in, (), (const, override));
            MOCK_METHOD(float, out, (), (const, override));
            MOCK_METHOD(float, rad_in, (), (const, override));
            MOCK_METHOD(float, rad_out, (), (const, override));
            MOCK_METHOD(float, range, (), (const, override));
            MOCK_METHOD(float, length, (), (const, override));
            MOCK_METHOD(float, cutoff, (), (const, override));
            MOCK_METHOD(float, radius, (), (const override));
            MOCK_METHOD(float, density, (), (const, override));
            MOCK_METHOD(void, set_position, (const DirectX::SimpleMath::Vector3&), (override));
            MOCK_METHOD(void, render_direction, (const ICamera&, const ILevelTextureStorage&), (override));

            std::shared_ptr<MockLight> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockLight> with_room(uint32_t number)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockLight> with_type(trlevel::LightType type)
            {
                ON_CALL(*this, type).WillByDefault(testing::Return(type));
                return shared_from_this();
            }
        };
    }
}
