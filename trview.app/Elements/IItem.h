#pragma once

#include <cstdint>
#include <string>
#include <external/DirectXTK/Inc/SimpleMath.h>
#include <trlevel/ILevel.h>
#include <trlevel/trtypes.h>
#include "../Geometry/IRenderable.h"
#include "../Geometry/PickResult.h"
#include "../Graphics/IMeshStorage.h"

namespace trview
{
    struct ITrigger;
    struct IItem : public IRenderable
    {
        using EntitySource =
            std::function<std::shared_ptr<IItem> (const trlevel::ILevel&, const trlevel::tr2_entity&, uint32_t, const std::vector<std::weak_ptr<ITrigger>>&, const IMeshStorage&, const std::weak_ptr<ILevel>&)>;
        using AiSource =
            std::function<std::shared_ptr<IItem>(const trlevel::ILevel&, const trlevel::tr4_ai_object&, uint32_t, const IMeshStorage&, const std::weak_ptr<ILevel>&)>;

        virtual ~IItem() = 0;
        virtual uint32_t number() const = 0;
        virtual uint16_t room() const = 0;
        virtual PickResult pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const = 0;
        virtual DirectX::BoundingBox bounding_box() const = 0;
        /// <summary>
        /// Adjust the y position of the entity by the specified amount.
        /// </summary>
        /// <param name="amount">The amount to translate byin the Y axis.</param>
        virtual void adjust_y(float amount) = 0;
        /// <summary>
        /// Determines whether the entity needs y adjustment based on the ocb value.
        /// </summary>
        /// <returns>True if the y needs to be adjusted.</returns>
        virtual bool needs_ocb_adjustment() const = 0;
        virtual std::string type() const = 0;
        virtual std::vector<std::weak_ptr<ITrigger>> triggers() const = 0;
        virtual uint32_t type_id() const = 0;
        virtual int32_t ocb() const = 0;
        virtual uint16_t activation_flags() const = 0;
        virtual bool clear_body_flag() const = 0;
        virtual bool invisible_flag() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual std::weak_ptr<ILevel> level() const = 0;
    };

    bool is_mutant_egg(const IItem& item);
    bool is_mutant_egg(uint32_t type_id);
    uint16_t mutant_egg_contents(const IItem& item);
    uint16_t mutant_egg_contents(uint16_t flags);
}
