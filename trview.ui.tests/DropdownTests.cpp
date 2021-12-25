#include <trview.ui/Dropdown.h>
#include <trview.ui/Listbox.h>
#include <trview.ui/JsonLoader.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(Dropdown, LoadFromJson)
{
    const std::string json = "{\"type\":\"dropdown\",\"text\":\"Test\",\"text_colour\":\"red\",\"text_background_colour\":\"blue\",\"values\":[\"One\",{\"name\":\"Two\",\"foreground_colour\":\"red\",\"background_colour\":\"white\"},\"Three\"],\"selected_value\":\"Two\"}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto dropdown = dynamic_cast<Dropdown*>(control.get());
    ASSERT_NE(dropdown, nullptr);

    const std::vector<Dropdown::Value> expected
    {
        { L"One" },
        { L"Two", Colour::Red, Colour::White },
        { L"Three" }
    };
    ASSERT_EQ(dropdown->values(), expected);
    ASSERT_EQ(dropdown->selected_value(), L"Two");
    ASSERT_EQ(dropdown->text_colour(), Colour::Red);
    ASSERT_EQ(dropdown->text_background_colour(), Colour::Blue);
}

TEST(Dropdown, SelectValue)
{
    ui::Window window(Size(100, 100), Colour::White);
    auto dropdown = window.add_child(std::make_unique<Dropdown>(Size(100, 20)));
    dropdown->set_values({ L"One", L"Two", L"Three" });
    dropdown->set_dropdown_scope(&window);

    std::optional<std::wstring> raised;
    auto token = dropdown->on_value_selected += [&](const auto& value)
    {
        raised = value;
    };

    auto listbox = dropdown->dropdown_listbox();
    ASSERT_NE(listbox, nullptr);
    ASSERT_FALSE(listbox->visible());

    auto button = dropdown->find<Button>(Dropdown::Names::dropdown_button);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(listbox->visible());

    auto row = listbox->find<Control>(Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);
    auto cell = row->find<Button>(Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell, nullptr);

    cell->clicked(Point());

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), L"Two");
    ASSERT_EQ(button->text(), L"Two");
    ASSERT_EQ(dropdown->selected_value(), L"Two");
    ASSERT_FALSE(listbox->visible());
}

TEST(Dropdown, ValueColours)
{
    ui::Window window(Size(100, 100), Colour::White);
    auto dropdown = window.add_child(std::make_unique<Dropdown>(Size(100, 20)));
    dropdown->set_values
    (
        {
            Dropdown::Value{ L"One", Colour::Red, Colour::White },
            Dropdown::Value{ L"Two", Colour::Green, Colour::Blue }
        }
    );
    dropdown->set_dropdown_scope(&window);

    auto listbox = dropdown->dropdown_listbox();
    ASSERT_NE(listbox, nullptr);
    ASSERT_FALSE(listbox->visible());

    auto button = dropdown->find<Button>(Dropdown::Names::dropdown_button);
    ASSERT_NE(button, nullptr);
    button->clicked(Point());

    ASSERT_TRUE(listbox->visible());

    auto row1 = listbox->find<Control>(Listbox::Names::row_name_format + "0");
    ASSERT_NE(row1, nullptr);
    auto cell1 = row1->find<Button>(Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell1, nullptr);
    ASSERT_EQ(cell1->text(), L"One");
    ASSERT_EQ(cell1->text_colour(), Colour::Red);
    ASSERT_EQ(cell1->text_background_colour(), Colour::White);

    auto row2 = listbox->find<Control>(Listbox::Names::row_name_format + "1");
    ASSERT_NE(row2, nullptr);
    auto cell2 = row2->find<Button>(Listbox::Row::Names::cell_name_format + "Name");
    ASSERT_NE(cell2, nullptr);
    ASSERT_EQ(cell2->text(), L"Two");
    ASSERT_EQ(cell2->text_colour(), Colour::Green);
    ASSERT_EQ(cell2->text_background_colour(), Colour::Blue);
}

