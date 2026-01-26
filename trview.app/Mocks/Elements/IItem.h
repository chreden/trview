#pragma once

#include "../../Elements/IItem.h"
#include <memory>

namespace trview
{
    namespace mocks
    {
        struct MockItem : public IItem, public std::enable_shared_from_this<MockItem>
        {
            MockItem();
            virtual ~MockItem();
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(std::weak_ptr<IRoom>, room, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(DirectX::BoundingBox, bounding_box, (), (const, override));
            MOCK_METHOD(void, adjust_y, (float), (override));
            MOCK_METHOD(bool, needs_ocb_adjustment, (), (const, override));
            MOCK_METHOD(std::string, type, (), (const, override));
            MOCK_METHOD(std::vector<std::weak_ptr<ITrigger>>, triggers, (), (const, override));
            MOCK_METHOD(uint32_t, type_id, (), (const, override));
            MOCK_METHOD(int32_t, ocb, (), (const, override));
            MOCK_METHOD(uint16_t, activation_flags, (), (const, override));
            MOCK_METHOD(bool, clear_body_flag, (), (const, override));
            MOCK_METHOD(bool, invisible_flag, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(int32_t, angle, (), (const, override));
            MOCK_METHOD(std::unordered_set<std::string>, categories, (), (const, override));
            MOCK_METHOD(void, set_categories, (const std::unordered_set<std::string>&), (override));
            MOCK_METHOD(std::optional<bool>, ng_plus, (), (const, override));
            MOCK_METHOD(void, set_ng_plus, (bool), (override));
            MOCK_METHOD(bool, is_ai, (), (const, override));
            MOCK_METHOD(bool, is_remastered_extra, (), (const, override));
            MOCK_METHOD(void, set_remastered_extra, (bool), (override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));

            bool _visible_state;

            std::shared_ptr<MockItem> with_ng_plus(std::optional<bool> value)
            {
                ON_CALL(*this, ng_plus).WillByDefault(testing::Return(value));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_number(uint32_t number)
            {
                ON_CALL(*this, number).WillByDefault(testing::Return(number));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_room(std::shared_ptr<IRoom> room)
            {
                ON_CALL(*this, room).WillByDefault(testing::Return(room));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_type_id(uint32_t type_id)
            {
                ON_CALL(*this, type_id).WillByDefault(testing::Return(type_id));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_activation_flags(uint16_t flags)
            {
                ON_CALL(*this, activation_flags).WillByDefault(testing::Return(flags));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
            {
                ON_CALL(*this, triggers).WillByDefault(testing::Return(triggers));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_visible(bool value)
            {
                ON_CALL(*this, visible).WillByDefault(testing::Return(value));
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_updating_visible(bool value)
            {
                _visible_state = value;
                ON_CALL(*this, visible).WillByDefault([&]() { return _visible_state; });
                ON_CALL(*this, set_visible).WillByDefault([&](auto v) { _visible_state = v; });
                return shared_from_this();
            }

            std::shared_ptr<MockItem> with_level(std::weak_ptr<ILevel> level)
            {
                ON_CALL(*this, level).WillByDefault(testing::Return(level));
                return shared_from_this();
            }
        };
    }
}
