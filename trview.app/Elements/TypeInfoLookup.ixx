export module trview.app:TypeInfoLookup;

import std;

import :ITypeInfoLookup;

namespace trview
{
    export class TypeInfoLookup : public ITypeInfoLookup
    {
    public:
        explicit TypeInfoLookup(const std::string& type_name_json, const std::optional<std::string>& extra_type_name_json);
        virtual ~TypeInfoLookup() = default;
        TypeInfo lookup(trlevel::PlatformAndVersion level_version, uint32_t type_id, int16_t flags) const override;
    private:
        std::unordered_map<std::string, std::unordered_map<uint32_t, TypeInfo>> _type_names;
    };
}
