#include "Item.h"

namespace trview
{
    Item::Item(uint32_t number, uint32_t room, const std::wstring& type)
        : _number(number), _room(room), _type(type)
    {
    }

    uint32_t Item::number() const
    {
        return _number;
    }

    uint32_t Item::room() const
    {
        return _room;
    }

    std::wstring Item::type() const
    {
        return _type;
    }
}
