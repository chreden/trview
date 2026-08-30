export module trview.common:Json;

import nlohmann.json;
import std;

namespace trview
{
    template <typename T>
    T read_attribute(const nlohmann::json& json, const std::string& attribute_name);

    template <typename T>
    T read_attribute(const nlohmann::json& json, const std::string& attribute_name, const T& default_value);

    template <typename T>
    void read_attribute(const nlohmann::json& json, T& destination, const std::string& attribute_name);

    template <typename T>
    void read_attribute(const nlohmann::json& json, std::optional<T>& destination, const std::string& attribute_name);
}

#include "Json.hpp"