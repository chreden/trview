#pragma once

#include <cstdint>
#include <string>
#include "Trigger.h"

namespace trview
{
    /// An item in a level.
    class Item final
    {
    public:
        /// Default constructor for item.
        Item() = default;

        /// Create an item.
        /// @param number The item number in the level.
        /// @param room The room number the item is in.
        /// @param type_id The type number of the item.
        /// @param type The type name of the item.
        /// @param ocb The OCB value of the item.
        /// @param flags The flags for the entity.
        /// @param triggers The triggers that affect this entity.
        explicit Item(uint32_t number, uint32_t room, const uint32_t type_id, const std::wstring& type, uint32_t ocb, uint16_t flags, const std::vector<Trigger*>& triggers);

        /// Get the item number.
        /// @returns The item number.
        uint32_t number() const;

        /// Get the room number.
        /// @returns The room the item is in.
        uint32_t room() const;

        /// Get the type id of the item.
        /// @returns The type of the item.
        uint32_t type_id() const;

        /// Get the type name of the item.
        /// @returns The type name of the item.
        const std::wstring& type() const;

        /// Get the OCB value of the item.
        /// @returns The OCB value of the item.
        uint32_t ocb() const;

        /// Get the activation flags for the entity.
        /// @returns The activation flags.
        uint16_t activation_flags() const;

        /// Get whether the clear_body flag is set.
        /// @returns Whether clear_body is set.
        bool clear_body_flag() const;

        /// Get whether the invisible_flag is set.
        /// @returns Whether invisible flag is set.
        bool invisible_flag() const;

        /// Get the triggers that affect this object.
        /// @returns The triggers.
        const std::vector<Trigger*>& triggers() const;
    private:
        std::vector<Trigger*> _triggers;
        uint32_t _number{ 0u };
        uint32_t _room{ 0u };
        uint32_t _type_id{ 0u };
        std::wstring _type;
        uint32_t _ocb{ 0u };
        uint16_t _flags{ 0u };
    };
}
