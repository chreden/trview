#pragma once

#include "ISoundSource.h"
#include <trlevel/LevelVersion.h>
#include "../../Geometry/IMesh.h"

namespace trview
{
    class SoundSource final : public ISoundSource, public std::enable_shared_from_this<ISoundSource>
    {
    public:
        explicit SoundSource(
            const std::shared_ptr<IMesh>& mesh,
            const std::shared_ptr<ITextureStorage>& texture_storage,
            uint32_t number,
            const trlevel::tr_sound_source& source,
            const std::optional<trlevel::tr_x_sound_details>& details,
            trlevel::LevelVersion level_version,
            const std::weak_ptr<ILevel>& level);
        virtual ~SoundSource() = default;
        uint16_t chance() const override;
        uint16_t characteristics() const override;
        uint16_t flags() const override;
        void get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        int16_t id() const override;
        uint32_t number() const override;
        PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const;
        uint8_t pitch() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        uint8_t range() const override;
        void render(const ICamera& camera, const DirectX::SimpleMath::Color& colour) override;
        std::optional<int16_t> sample() const override;
        void set_visible(bool value) override;
        bool visible() const override;
        uint16_t volume() const override;
        std::weak_ptr<ILevel> level() const override;
        int32_t filterable_index() const override;
    private:
        uint16_t _chance{ 0u };
        uint16_t _characteristics{ 0u };
        uint16_t _flags{ 0u };
        int16_t _id{ 0 };
        std::shared_ptr<IMesh> _mesh;
        uint32_t _number{ 0u };
        uint8_t _pitch{ 0u };
        DirectX::SimpleMath::Vector3 _position;
        uint8_t _range{ 0u };
        std::optional<int16_t> _sample;
        graphics::Texture _sound_texture;
        bool _visible{ true };
        uint16_t _volume{ 0u };
        std::weak_ptr<ILevel> _level;
    };
}

