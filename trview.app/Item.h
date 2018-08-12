#pragma once

#include <cstdint>
#include <string>

namespace trview
{
    /// An item in a level.
    class Item final
    {
    public:
        /// Create an item.
        /// @param number The item number in the level.
        /// @param room The room number the item is in.
        /// @param type The type name of the item.
        explicit Item(uint32_t number, uint32_t room, const std::wstring& type);

        /// Get the item number.
        /// @returns The item number.
        uint32_t number() const;

        /// Get the room number.
        /// @returns The room the item is in.
        uint32_t room() const;

        /// Get the type name of the item.
        /// @returns The type of the item.
        std::wstring type() const;
    private:
        uint32_t _number;
        uint32_t _room;
        std::wstring _type;
    };
}
