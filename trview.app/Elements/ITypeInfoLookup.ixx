export module trview.app:ITypeInfoLookup;

import std;
import std.compat;

import trlevel;

import :TypeInfo;

namespace trview
{
    export struct ITypeInfoLookup
    {
    public:
        virtual ~ITypeInfoLookup() = 0;
        virtual TypeInfo lookup(trlevel::PlatformAndVersion level_version, uint32_t type_id, int16_t flags) const = 0;
    };
}
