#pragma once

#include <trview.app/Elements/IStaticMesh.h>

namespace trview
{
    namespace mocks
    {
        struct MockStaticMesh : public IStaticMesh, public std::enable_shared_from_this<MockStaticMesh>
        {
            MockStaticMesh();
            virtual ~MockStaticMesh();
            MOCK_METHOD(DirectX::BoundingBox, collision, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render_bounding_box, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (), (const, override));
            MOCK_METHOD(float, rotation, (), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, visibility, (), (const, override));
            MOCK_METHOD(Type, type, (), (const, override));
            MOCK_METHOD(uint16_t, id, (), (const, override));
            MOCK_METHOD(void, set_number, (uint32_t), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(bool, breakable, (), (const, override));
            MOCK_METHOD(bool, has_collision, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));

            std::shared_ptr<MockStaticMesh> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockStaticMesh> with_visible(bool value)
            {
                ON_CALL(*this, visible).WillByDefault(testing::Return(value));
                return shared_from_this();
            }

            std::shared_ptr<MockStaticMesh> with_room(std::shared_ptr<IRoom> room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }
        };
    }
}
