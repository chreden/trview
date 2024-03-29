#include "RandomizerSettings.h"
#include <trview.common/Json.h>

using namespace nlohmann;

namespace trview
{
    namespace
    {
        template < typename T, typename... U >
        std::vector<T> to_vector(const std::vector<std::variant<U...>>& variants)
        {
            std::vector<T> result;
            for (const auto& v : variants)
            {
                result.push_back(std::get<T>(v));
            }
            return result;
        }
    }

    void to_json(json& json, const RandomizerSettings::Setting::Type& type)
    {
        switch (type)
        {
            case RandomizerSettings::Setting::Type::Boolean:
            {
                json = "boolean";
                break;
            }
            case RandomizerSettings::Setting::Type::String:
            {
                json = "string";
                break;
            }
            case RandomizerSettings::Setting::Type::Number:
            {
                json = "number";
                break;
            }
        }
    }

    void from_json(const json& json, RandomizerSettings::Setting::Type& type)
    {
        const std::string type_string = json.get<std::string>();
        if (type_string == "boolean")
        {
            type = RandomizerSettings::Setting::Type::Boolean;
        }
        else if (type_string == "string")
        {
            type = RandomizerSettings::Setting::Type::String;
        }
        else if (type_string == "number")
        {
            type = RandomizerSettings::Setting::Type::Number;
        }
    }

    void to_json(json& json, const RandomizerSettings::Setting& setting)
    {
        json["display"] = setting.display;
        json["type"] = setting.type;
        json["always_output"] = setting.always_output;

        switch (setting.type)
        {
        case RandomizerSettings::Setting::Type::Boolean:
        {
            json["default"] = std::get<bool>(setting.default_value);
            break;
        }
        case RandomizerSettings::Setting::Type::String:
        {
            json["default"] = std::get<std::string>(setting.default_value);
            break;
        }
        case RandomizerSettings::Setting::Type::Number:
        {
            json["default"] = std::get<float>(setting.default_value);
            break;
        }
        }

        if (!setting.options.empty())
        {
            switch (setting.type)
            {
                case RandomizerSettings::Setting::Type::String:
                {
                    json["options"] = to_vector<std::string>(setting.options);
                    break;
                }
            }
        }
    }

    void from_json(const json& json, RandomizerSettings::Setting& setting)
    {
        setting.display = read_attribute<std::string>(json, "display");
        setting.type = read_attribute<RandomizerSettings::Setting::Type>(json, "type");
        setting.always_output = read_attribute<bool>(json, "always_output", false);
        switch (setting.type)
        {
            case RandomizerSettings::Setting::Type::Boolean:
            {
                setting.default_value = read_attribute<bool>(json, "default");
                break;
            }
            case RandomizerSettings::Setting::Type::String:
            {
                setting.default_value = read_attribute<std::string>(json, "default");
                if (json.count("options"))
                {
                    std::vector<std::string> options = json["options"];
                    std::copy(options.begin(), options.end(), std::back_inserter(setting.options));
                }
                break;
            }
            case RandomizerSettings::Setting::Type::Number:
            {
                setting.default_value = read_attribute<float>(json, "default");
                break;
            }
        }
    }

    void to_json(json& json, const RandomizerSettings& settings)
    {
        json["fields"] = settings.settings;
    }

    void from_json(const json& json, RandomizerSettings& settings)
    {
        const auto& fields = json["fields"];
        for (auto it = fields.begin(); it != fields.end(); ++it)
        {
            settings.settings[it.key()] = it.value().get<RandomizerSettings::Setting>();
        }
    }

    uint32_t RandomizerSettings::settings_of_type(Setting::Type type) const
    {
        return static_cast<uint32_t>(std::count_if(settings.begin(), settings.end(),
            [type](const auto& t) { return t.second.type == type; }));
    }
}
