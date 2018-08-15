#include "Item.h"

namespace trview
{
    Item::Item(uint32_t number, uint32_t room, uint32_t type_id, const std::wstring& type)
        : _number(number), _room(room), _type_id(type_id), _type(type)
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

    uint32_t Item::type_id() const
    {
        return _type_id;
    }

    const std::wstring& Item::type() const
    {
        return _type;
    }

}
