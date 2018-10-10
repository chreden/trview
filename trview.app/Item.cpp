#include "Item.h"

namespace trview
{
    Item::Item(uint32_t number, uint32_t room, uint32_t type_id, const std::wstring& type, uint32_t ocb, uint16_t flags, const std::vector<Trigger*>& triggers)
        : _number(number), _room(room), _type_id(type_id), _type(type), _ocb(ocb), _flags(flags), _triggers(triggers)
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

    uint32_t Item::ocb() const
    {
        return _ocb;
    }

    uint16_t Item::activation_flags() const
    {
        return (_flags & 0x3E00) >> 9;
    }

    bool Item::clear_body_flag() const
    {
        return (_flags & 0x8000) != 0;
    }

    bool Item::invisible_flag() const
    {
        return (_flags & 0x100) != 0;
    }

    const std::vector<Trigger*>& Item::triggers() const
    {
        return _triggers;
    }
}
