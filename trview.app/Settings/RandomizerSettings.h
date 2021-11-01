#pragma once

#include <map>
#include <variant>
#include <external/nlohmann/json.hpp>

namespace trview
{
    struct RandomizerSettings
    {
        struct Setting
        {
            enum class Type
            {
                Boolean,
                String,
                Number
            };

            /// <summary>
            /// The name used to show this variable in the UI.
            /// </summary>
            std::string display;
            /// <summary>
            /// The type of the setting.
            /// </summary>
            Type type;
            /// <summary>
            /// The default value to use.
            /// </summary>
            std::variant<bool, std::string> default_value;
            /// <summary>
            /// Options allowed when this is a select element. If there are no options
            /// then this isn't a select element.
            /// </summary>
            std::vector<std::variant<bool, std::string>> options;
        };

        std::map<std::string, Setting> settings;
    };

    void to_json(nlohmann::json& json, const RandomizerSettings& settings);
    void from_json(const nlohmann::json& json, RandomizerSettings& settings);
}
