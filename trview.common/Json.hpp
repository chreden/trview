#pragma once

namespace trview
{
    template <typename T>
    T read_attribute(const nlohmann::json& json, const std::string& attribute_name)
    {
        if (json.count(attribute_name) != 0)
        {
            return json[attribute_name].get<T>();
        }
        return {};
    }

    template <typename T>
    T read_attribute(const nlohmann::json& json, const std::string& attribute_name, const T& default_value)
    {
        if (json.count(attribute_name) != 0)
        {
            return json[attribute_name].get<T>();
        }
        return default_value;
    }

    template <typename T>
    void read_attribute(const nlohmann::json& json, T& destination, const std::string& attribute_name)
    {
        if (json.count(attribute_name) != 0)
        {
            destination = json[attribute_name].get<T>();
        }
    }

    template <typename T>
    void read_attribute(const nlohmann::json& json, std::optional<T>& destination, const std::string& attribute_name)
    {
        if (json.count(attribute_name) != 0)
        {
            destination = json[attribute_name].get<T>();
        }
    }
}