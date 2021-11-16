#include "json.h"
#include <external/nlohmann/json.hpp>
#include <trview.common/Json.h>
#include <trview.common/Resources.h>
#include <trview.common/JsonSerializers.h>

#include "Layouts/FreeLayout.h"
#include "Layouts/StackLayout.h"
#include "Layouts/GridLayout.h"

#include "Window.h"
#include "Label.h"
#include "Button.h"
#include "Checkbox.h"
#include "GroupBox.h"
#include "NumericUpDown.h"
#include "Slider.h"

namespace trview
{
    namespace ui
    {
        namespace
        {
            std::unique_ptr<ILayout> parse_layout(const nlohmann::json& json)
            {
                auto type = read_attribute<std::string>(json, "type");
                if (type == "stack")
                {
                    auto size_mode = read_attribute<std::string>(json, "size_mode", "auto");
                    auto padding = read_attribute<float>(json, "padding", 0.0f);
                    auto direction = read_attribute<std::string>(json, "direction", "vertical");
                    auto layout = std::make_unique<StackLayout>(padding,
                        direction == "vertical" ? StackLayout::Direction::Vertical : StackLayout::Direction::Horizontal,
                        size_mode == "auto" ? SizeMode::Auto : SizeMode::Manual);
                    layout->set_margin(read_attribute<Size>(json, "margin", Size()));
                    auto size_dimension = read_attribute<std::string>(json, "auto_size_dimension", "all");
                    layout->set_auto_size_dimension(
                        size_dimension == "all" ? SizeDimension::All : size_dimension == "width" ? SizeDimension::Width : SizeDimension::Height);
                    return std::move(layout);
                }
                else if (type == "grid")
                {
                    auto columns = read_attribute<uint32_t>(json, "columns", 1u);
                    auto rows = read_attribute<uint32_t>(json, "rows", 1u);
                    return std::make_unique<GridLayout>(columns, rows);
                }
                return std::make_unique<FreeLayout>();
            }

            std::unique_ptr<Control> parse_json(const nlohmann::json& json)
            {
                auto type = read_attribute<std::string>(json, "type");
                auto position = read_attribute<Point>(json, "position", Point());
                auto size = read_attribute<Size>(json, "size", Size());
                auto colour = read_attribute<Colour>(json, "background_colour", Colour::Transparent);

                std::unique_ptr<Control> control;
                if (type == "window")
                {
                    control = std::make_unique<ui::Window>(position, size, colour);
                }
                else if (type == "label")
                {
                    auto text = read_attribute<std::string>(json, "text", std::string());
                    auto text_size = read_attribute<int>(json, "text_size", 8);
                    auto size_mode = read_attribute<std::string>(json, "size_mode", "manual");
                    auto text_alignment = read_attribute<std::string>(json, "text_alignment", "left");
                    auto paragraph_alignment = read_attribute<std::string>(json, "paragraph_alignment", "near");
                    control = std::make_unique<Label>(position, size, colour, to_utf16(text), text_size, 
                        text_alignment == "left" ? graphics::TextAlignment::Left : graphics::TextAlignment::Centre, // TODO: Parse all
                        paragraph_alignment == "near" ? graphics::ParagraphAlignment::Near : graphics::ParagraphAlignment::Centre, // TODO: Parse all
                        size_mode == "auto" ? SizeMode::Auto : SizeMode::Manual);
                }
                else if (type == "button")
                {
                    auto text = read_attribute<std::string>(json, "text", std::string());
                    control = std::make_unique<Button>(position, size, to_utf16(text));
                }
                else if (type == "checkbox")
                {
                    auto text = read_attribute<std::string>(json, "text", std::string());
                    control = std::make_unique<Checkbox>(colour, to_utf16(text));
                }
                else if (type == "groupbox")
                {
                    auto text = read_attribute<std::string>(json, "text", std::string());
                    auto border_colour = read_attribute<Colour>(json, "border_colour", Colour::Transparent);
                    control = std::make_unique<GroupBox>(position, size, colour, border_colour, to_utf16(text));
                }
                else if (type == "numericupdown")
                {
                    auto min_value = read_attribute<int>(json, "min_value", 0);
                    auto max_value = read_attribute<int>(json, "max_value", 100);
                    control = std::make_unique<NumericUpDown>(position, size, colour, min_value, max_value);
                }
                else if (type == "slider")
                {
                    control = std::make_unique<Slider>(position, size);
                }

                control->set_name(read_attribute<std::string>(json, "name", std::string()));
                control->set_visible(read_attribute<bool>(json, "visible", true));
                control->set_horizontal_alignment(read_attribute<std::string>(json, "horizontal_alignment", "near") == "near" ? Align::Near : Align::Centre);

                if (json.count("layout") != 0)
                {
                    control->set_layout(parse_layout(json["layout"]));
                }

                // Load child controls.
                if (json.count("children") != 0)
                {
                    for (const auto& child : json["children"])
                    {
                        control->add_child(parse_json(child));
                    }
                }

                return control;
            }
        }

        std::unique_ptr<Control> load_from_json(const std::string& json)
        {
            try
            {
                auto data = nlohmann::json::parse(json);
                return parse_json(data);
            }
            catch(const std::exception& e)
            {
                OutputDebugStringA(e.what());
                return nullptr;
            }
        }

        std::unique_ptr<Control> load_from_resource(uint32_t resource_id)
        {
            auto data = get_resource_memory(resource_id, L"TEXT");
            return ui::load_from_json(std::string(data.data, data.data + data.size));
        }
    }
}
