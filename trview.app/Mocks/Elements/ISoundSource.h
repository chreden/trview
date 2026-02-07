#pragma once

#include <trview.app/Elements/SoundSource/ISoundSource.h>

namespace trview
{
    namespace mocks
    {
        struct MockSoundSource : public ISoundSource, public std::enable_shared_from_this<MockSoundSource>
        {
            MockSoundSource();
            ~MockSoundSource();
            MOCK_METHOD(uint16_t, chance, (), (const, override));
            MOCK_METHOD(uint16_t, characteristics, (), (const, override));
            MOCK_METHOD(uint16_t, flags, (), (const, override));
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(int16_t, id, (), (const, override));
            MOCK_METHOD(uint32_t, number, (), (const, override));
            MOCK_METHOD(PickResult, pick, (const DirectX::SimpleMath::Vector3&, const DirectX::SimpleMath::Vector3&), (const, override));
            MOCK_METHOD(uint8_t, pitch, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(uint8_t, range, (), (const, override));
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(std::optional<int16_t>, sample, (), (const, override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
            MOCK_METHOD(uint16_t, volume, (), (const, override));
            MOCK_METHOD(std::weak_ptr<ILevel>, level, (), (const, override));
            MOCK_METHOD(int32_t, filterable_index, (), (const, override));
        };
    }
}