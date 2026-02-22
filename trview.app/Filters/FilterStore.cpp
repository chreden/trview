#include "FilterStore.h"
#include "../Messages/Messages.h"

#include <trview.common/Json.h>
#include <trview.common/Strings.h>
#include <trview.common/JsonSerializers.h>

namespace trview
{
    void from_json(const nlohmann::json& json, CompareOp& op)
    {
        const std::string value = to_lowercase(json.get<std::string>());
        if (value == "equal")
        {
            op = CompareOp::Equal;
        }
        else if (value == "notequal")
        {
            op = CompareOp::NotEqual;
        }
        else if (value == "greaterthan")
        {
            op = CompareOp::GreaterThan;
        }
        else if (value == "greaterthanorqqual")
        {
            op = CompareOp::GreaterThanOrEqual;
        }
        else if (value == "lessthan")
        {
            op = CompareOp::LessThan;
        }
        else if (value == "lessthanorequal")
        {
            op = CompareOp::LessThanOrEqual;
        }
        else if (value == "between")
        {
            op = CompareOp::Between;
        }
        else if (value == "betweeninclusive")
        {
            op = CompareOp::BetweenInclusive;
        }
        else if (value == "exists")
        {
            op = CompareOp::Exists;
        }
        else if (value == "notexists")
        {
            op = CompareOp::NotExists;
        }
        else if (value == "startswith")
        {
            op = CompareOp::StartsWith;
        }
        else if (value == "endswith")
        {
            op = CompareOp::EndsWith;
        }
        else if (value == "matches")
        {
            op = CompareOp::Matches;
        }
        else
        {
            op = CompareOp::Equal;
        }
    }

    void from_json(const nlohmann::json& json, Op& op)
    {
        const std::string value = to_lowercase(json.get<std::string>());
        if (value == "and")
        {
            op = Op::And;
        }
        else if (value == "or")
        {
            op = Op::Or;
        }
        else
        {
            op = Op::And;
        }
    }

    void from_json(const nlohmann::json& json, Filters::Filter& filter)
    {
        read_attribute(json, filter.key, "key");
        read_attribute(json, filter.compare, "compare");
        read_attribute(json, filter.value, "value");
        read_attribute(json, filter.value2, "value2");
        read_attribute(json, filter.op, "op");
        read_attribute(json, filter.invert, "invert");
        read_attribute(json, filter.type_key, "type_key");
        read_attribute(json, filter.children, "children");
    }

    void to_json(nlohmann::json& json, const CompareOp& op)
    {
        std::string value;
        switch (op)
        {
            case CompareOp::Equal:
                value = "Equal";
                break;
            case CompareOp::NotEqual:
                value = "NotEqual";
                break;
            case CompareOp::GreaterThan:
                value = "GreaterThan";
                break;
            case CompareOp::GreaterThanOrEqual:
                value = "GreaterThanOrEqual";
                break;
            case CompareOp::LessThan:
                value = "LessThan";
                break;
            case CompareOp::LessThanOrEqual:
                value = "LessThanOrEqual";
                break;
            case CompareOp::Between:
                value = "Between";
                break;
            case CompareOp::BetweenInclusive:
                value = "BetweenInclusive";
                break;
            case CompareOp::Exists:
                value = "Exists";
                break;
            case CompareOp::NotExists:
                value = "NotExists";
                break;
            case CompareOp::StartsWith:
                value = "StartsWith";
                break;
            case CompareOp::EndsWith:
                value = "EndsWith";
                break;
            case CompareOp::Matches:
                value = "Matches";
                break;
        }
        json = value;
    }

    void to_json(nlohmann::json& json, const Op& op)
    {
        std::string value;
        switch (op)
        {
        case Op::And:
            value = "And";
            break;
        case Op::Or:
            value = "Or";
            break;
        }
        json = value;
    }

    void to_json(nlohmann::json& json, const Filters::Filter& filter)
    {
        json["key"] = filter.key;
        json["compare"] = filter.compare;
        json["value"] = filter.value;
        json["value2"] = filter.value2;
        json["children"] = filter.children;
        json["op"] = filter.op;
        json["invert"] = filter.invert;
        json["type_key"] = filter.type_key;
    }

    IFilterStore::~IFilterStore()
    {
    }

    FilterStore::FilterStore(const std::shared_ptr<IFiles>& files, const UserSettings& settings)
        : _files(files), _settings(settings)
    {
    }

    void FilterStore::add(const std::string& key, const Filters::Filter& filter)
    {
        _filters.push_back(
            {
                .name = key,
                .filename = "C:\\dev\\trview-filters\\new.json",
                .filter = filter
            });
    }

    void FilterStore::load()
    {
        // TODO: Load filters from files in the directories.
        //       For now let's just use the appdata directory.
        if (!_settings.filter_directory.empty())
        {
            const auto files = _files->get_files(_settings.filter_directory, "\\*.json");
            for (const auto& file : files)
            {
                try
                {
                    const auto data = _files->load_file(file.path);
                    if (!data)
                    {
                        continue;
                    }

                    auto json = nlohmann::json::parse(data.value().begin(), data.value().end(), nullptr, true, true, true);

                    StoredFilter new_filter{ .filename = file.path };
                    read_attribute(json, new_filter.name, "name");
                    read_attribute(json, new_filter.filter, "filter");

                    _filters.push_back(new_filter);
                }
                catch (...)
                {
                }
            }
        }
    }

    std::map<std::string, Filters::Filter> FilterStore::filters() const
    {
        std::map<std::string, Filters::Filter> results;
        for (const auto& filter : _filters)
        {
            results[filter.name] = filter.filter;
        }
        return results;
    }

    std::map<std::string, Filters::Filter> FilterStore::filters_for_key(const std::string& key) const
    {
        std::map<std::string, Filters::Filter> results;
        for (const auto& filter : _filters)
        {
            if (filter.filter.type_key == key)
            {
                results[filter.name] = filter.filter;
            }
        }
        return results;
    }

    void FilterStore::receive_message(const Message& message)
    {
        if (auto settings = messages::read_settings(message))
        {
            _settings = settings.value();
        }
    }

    void FilterStore::save()
    {
        const auto dir = _settings.filter_directory.empty() ?
            (_files->appdata_directory() + "\\trview\\filters") : _settings.filter_directory;
        _files->create_directory(dir);

        for (const auto& filter : _filters)
        {
            try
            {
                nlohmann::json json;
                json["name"] = filter.name;
                json["filter"] = filter.filter;
                _files->save_file(dir + "\\" + filter.name + ".json", json.dump());
            }
            catch (...)
            {
            }
        }
    }
}
