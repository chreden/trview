#include "json.h"
#include <external/nlohmann/json.hpp>
#include <trview.common/Json.h>
#include <trview.common/Resources.h>
#include <trview.common/JsonSerializers.h>
#include <trview.graphics/JsonSerializers.h>
#include "JsonSerializers.h"

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
#include "Listbox.h"
#include "Dropdown.h"
#include "TextArea.h"
#include "Image.h"

using namespace trview::graphics;

namespace trview
{
    namespace ui
    {
        namespace
        {
            std::unique_ptr<ILayout> parse_layout(const nlohmann::json& json)
            {
                const auto type = read_attribute<std::string>(json, "type");
                if (type == "stack")
                {
                    const auto size_mode = read_attribute<SizeMode>(json, "size_mode", SizeMode::Auto);
                    const auto direction = read_attribute<StackLayout::Direction>(json, "direction", StackLayout::Direction::Vertical);
                    const auto padding = read_attribute<float>(json, "padding", 0.0f);
                    auto layout = std::make_unique<StackLayout>(padding, direction, size_mode);
                    layout->set_margin(read_attribute<Size>(json, "margin", Size()));
                    const auto size_dimension = read_attribute<SizeDimension>(json, "auto_size_dimension", SizeDimension::All);
                    layout->set_auto_size_dimension(size_dimension);
                    return std::move(layout);
                }
                else if (type == "grid")
                {
                    const auto columns = read_attribute<uint32_t>(json, "columns", 1u);
                    const auto rows = read_attribute<uint32_t>(json, "rows", 1u);
                    return std::make_unique<GridLayout>(columns, rows);
                }
                return std::make_unique<FreeLayout>();
            }

            Colour read_colour(const nlohmann::json& json, const std::string& attribute_name, const std::unordered_map<std::string, Colour>& named_colours, const Colour& default_colour)
            {
                if (json.count(attribute_name) == 0)
                {
                    return default_colour;
                }

                if (json[attribute_name].is_string())
                {
                    const auto key = json[attribute_name].get<std::string>();
                    const auto value = named_colours.find(key);
                    if (value != named_colours.end())
                    {
                        return value->second;
                    }
                }

                return json[attribute_name].get<Colour>();
            }

            std::unique_ptr<Control> parse_json(const nlohmann::json& json, std::unordered_map<std::string, Colour> named_colours)
            {
                if (json.count("colours") != 0)
                {
                    for (auto& [key, value] : json["colours"].items())
                    {
                        named_colours[key] = value.get<Colour>();
                    }
                }

                const auto type = read_attribute<std::string>(json, "type");
                const auto position = read_attribute<Point>(json, "position", Point());
                const auto size = read_attribute<Size>(json, "size", Size());
                const auto colour = read_colour(json, "background_colour", named_colours, Colour::Transparent);
                const auto text_background_colour = read_colour(json, "text_background_colour", named_colours, Colour(0.25f, 0.25f, 0.25f));
                const auto text_colour = read_colour(json, "text_colour", named_colours, Colour::White);

                std::unique_ptr<Control> control;
                if (type == "window")
                {
                    control = std::make_unique<ui::Window>(position, size, colour);
                }
                else if (type == "label")
                {
                    const auto text = read_attribute<std::string>(json, "text", std::string());
                    const auto text_size = read_attribute<int>(json, "text_size", 8);
                    const auto size_mode = read_attribute<SizeMode>(json, "size_mode", SizeMode::Manual);
                    const auto text_alignment = read_attribute<TextAlignment>(json, "text_alignment", TextAlignment::Left);
                    const auto paragraph_alignment = read_attribute<ParagraphAlignment>(json, "paragraph_alignment", ParagraphAlignment::Near);
                    control = std::make_unique<Label>(position, size, colour, to_utf16(text), text_size, text_alignment, paragraph_alignment, size_mode);
                }
                else if (type == "button")
                {
                    const auto text = read_attribute<std::string>(json, "text", std::string());
                    auto button = std::make_unique<Button>(position, size, to_utf16(text));
                    button->set_text_background_colour(text_background_colour);
                    button->set_text_colour(text_colour);
                    control = std::move(button);
                }
                else if (type == "checkbox")
                {
                    const auto text = read_attribute<std::string>(json, "text", std::string());
                    const auto state = read_attribute<bool>(json, "state", false);
                    auto checkbox = std::make_unique<Checkbox>(colour, to_utf16(text));
                    checkbox->set_state(state);
                    control = std::move(checkbox);
                }
                else if (type == "groupbox")
                {
                    const auto text = read_attribute<std::string>(json, "text", std::string());
                    const auto border_colour = read_colour(json, "border_colour", named_colours, Colour::Transparent);
                    control = std::make_unique<GroupBox>(position, size, colour, border_colour, to_utf16(text));
                }
                else if (type == "numericupdown")
                {
                    const auto min_value = read_attribute<int>(json, "min_value", 0);
                    const auto max_value = read_attribute<int>(json, "max_value", 100);
                    const auto value = read_attribute<int>(json, "value", min_value);
                    auto updown = std::make_unique<NumericUpDown>(position, size, colour, min_value, max_value);
                    updown->set_value(value);
                    control = std::move(updown);
                }
                else if (type == "slider")
                {
                    control = std::make_unique<Slider>(position, size);
                }
                else if (type == "listbox")
                {
                    auto listbox = std::make_unique<Listbox>(position, size, colour);
                    listbox->set_show_headers(read_attribute<bool>(json, "show_headers", true));
                    listbox->set_show_scrollbar(read_attribute<bool>(json, "show_scrollbar", true));
                    listbox->set_show_highlight(read_attribute<bool>(json, "show_highlight", true));
                    listbox->set_enable_sorting(read_attribute<bool>(json, "enable_sorting", true));

                    if (json.count("columns") != 0)
                    {
                        std::vector<Listbox::Column> columns;
                        for (const auto& col : json["columns"])
                        {
                            const auto name = read_attribute<std::string>(col, "name");
                            const auto type = read_attribute<Listbox::Column::Type>(col, "type", Listbox::Column::Type::String);
                            const auto width = read_attribute<uint32_t>(col, "width");
                            const auto identity = read_attribute<Listbox::Column::IdentityMode>(col, "identity", Listbox::Column::IdentityMode::Key);
                            columns.push_back(Listbox::Column(identity, type, to_utf16(name), width));
                        }
                        listbox->set_columns(columns);
                    }

                    control = std::move(listbox);
                }
                else if (type == "dropdown")
                {
                    std::vector<Dropdown::Value> values;
                    if (json.count("values"))
                    {
                        for (const auto& value : json["values"])
                        {
                            Dropdown::Value new_value;
                            if (value.is_string())
                            {
                                new_value.text = to_utf16(value.get<std::string>());
                            }
                            else
                            {
                                new_value.text = to_utf16(read_attribute<std::string>(value, "name"));
                                new_value.background = read_colour(value, "background_colour", named_colours, new_value.background);
                                new_value.foreground = read_colour(value, "foreground_colour", named_colours, new_value.foreground);
                            }
                            values.push_back(new_value);
                        }
                    }

                    auto dropdown = std::make_unique<Dropdown>(position, size);
                    dropdown->set_values(values);
                    dropdown->set_text_background_colour(text_background_colour);
                    dropdown->set_text_colour(text_colour);

                    if (json.count("selected_value") && json["selected_value"].is_string())
                    {
                        dropdown->set_selected_value(to_utf16(json["selected_value"]));
                    }
                    else if (!values.empty())
                    {
                        dropdown->set_selected_value(values[0].text);
                    }
                    control = std::move(dropdown);
                }
                else if (type == "textarea")
                {
                    const auto text_alignment = read_attribute<TextAlignment>(json, "text_alignment", TextAlignment::Left);
                    auto text_area = std::make_unique<TextArea>(position, size, colour, text_colour, text_alignment);
                    text_area->set_scrollbar_visible(read_attribute<bool>(json, "show_scrollbar", true));
                    text_area->set_read_only(read_attribute<bool>(json, "read_only", false));
                    const auto line_mode = read_attribute<TextArea::Mode>(json, "line_mode", TextArea::Mode::MultiLine);
                    text_area->set_mode(line_mode);
                    text_area->set_text(to_utf16(read_attribute<std::string>(json, "text", "")));
                    control = std::move(text_area);
                }
                else if (type == "image")
                {
                    control = std::make_unique<Image>(position, size);
                }

                control->set_name(read_attribute<std::string>(json, "name", std::string()));
                control->set_visible(read_attribute<bool>(json, "visible", true));
                control->set_horizontal_alignment(read_attribute<Align>(json, "horizontal_alignment", Align::Near));
                control->set_vertical_alignment(read_attribute<Align>(json, "vertical_alignment", Align::Near));

                if (json.count("layout") != 0)
                {
                    control->set_layout(parse_layout(json["layout"]));
                }

                // Load child controls.
                if (json.count("children") != 0)
                {
                    for (const auto& child : json["children"])
                    {
                        control->add_child(parse_json(child, named_colours));
                    }
                }

                return control;
            }
        }

        std::unique_ptr<Control> load_from_json(const std::string& json)
        {
            try
            {
                std::unordered_map<std::string, Colour> named_colours;
                auto data = nlohmann::json::parse(json);
                return parse_json(data, named_colours);
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
