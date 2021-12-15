#include "JsonSerializers.h"

namespace trview
{
    namespace ui
    {
        void from_json(const nlohmann::json& json, SizeMode& mode)
        {
            const auto text = json.get<std::string>();
            mode = text == "auto" ? SizeMode::Auto : SizeMode::Manual;
        }

        void from_json(const nlohmann::json& json, StackLayout::Direction& direction)
        {
            const auto text = json.get<std::string>();
            direction = text == "horizontal" ? StackLayout::Direction::Horizontal : StackLayout::Direction::Vertical;
        }

        void from_json(const nlohmann::json& json, Listbox::Column::IdentityMode& identity_mode)
        {
            const auto text = json.get<std::string>();
            identity_mode = text == "key" ? Listbox::Column::IdentityMode::Key : Listbox::Column::IdentityMode::None;
        }

        void from_json(const nlohmann::json& json, Listbox::Column::Type& type)
        {
            const auto text = json.get<std::string>();
            if (text == "number")
            {
                type = Listbox::Column::Type::Number;
            }
            else if (text == "boolean")
            {
                type = Listbox::Column::Type::Boolean;
            }
            else
            {
                type = Listbox::Column::Type::String;
            }
        }

        void from_json(const nlohmann::json& json, TextArea::Mode& mode)
        {
            const auto text = json.get<std::string>();
            if (text == "multi")
            {
                mode = TextArea::Mode::MultiLine;
            }
            else
            {
                mode = TextArea::Mode::SingleLine;
            }
        }

        void from_json(const nlohmann::json& json, Align& align)
        {
            const auto text = json.get<std::string>();
            if (text == "centre")
            {
                align = Align::Centre;
            }
            else
            {
                align = Align::Near;
            }
        }

        void from_json(const nlohmann::json& json, SizeDimension& dimension)
        {
            const auto text = json.get<std::string>();
            if (text == "width")
            {
                dimension = SizeDimension::Width;
            }
            else if (text == "height")
            {
                dimension = SizeDimension::Height;
            }
            else if (text == "all")
            {
                dimension = SizeDimension::All;
            }
            else
            {
                dimension = SizeDimension::None;
            }
        }
    }
}
