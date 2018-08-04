#pragma once

#include <cstdint>
#include <string>

namespace trview
{
    class Item final
    {
    public:
        explicit Item(uint32_t number, uint32_t room, const std::wstring& type);

        uint32_t number() const;

        uint32_t room() const;

        std::wstring type() const;
    private:
        uint32_t _number;
        uint32_t _room;
        std::wstring _type;
    };
}
