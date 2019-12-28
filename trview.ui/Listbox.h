/// @file Listbox.h
/// @brief UI element that displays a number of rows in a grid with column headers.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <optional>

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
        class Scrollbar;

        /// Displays a number of rows in a grid with column headers.
        class Listbox : public StackPanel
        {
        public:
            /// A column in a list box.
            class Column final
            {
            public:
                /// The data type that will be in this column. Determines how the data will be sorted.
                enum class Type
                {
                    /// The data is a string and will be sorted alphabetically.
                    String,
                    /// The data is numerical and will be sorted numerically.
                    Number
                };

                /// Default constructor.
                Column();

                /// Create a column.
                /// @param type The type of the column.
                /// @param name The column name. This is displayed as a header.
                /// @param width The width of the column.
                Column(Type type, const std::wstring& name, uint32_t width);

                /// Get the name of the column. This is displayed as a header.
                /// @returns The name of the column.
                const std::wstring& name() const;

                /// Get the type of the column data.
                /// @returns The data type of the column.
                Type type() const;

                /// Get the width of the column.
                /// @returns The width of the column.
                uint32_t width() const;
            private:
                std::wstring _name;
                Type _type;
                uint32_t _width;
            };

            /// An entry in the list box.
            class Item final
            {
            public:
                /// Create an item.
                /// @param values The column index values for the item.
                Item(const std::unordered_map<std::wstring, std::wstring>& values);

                /// Create an item.
                /// @param values The column index values for the item.
                /// @param foreground The foreground text colour.
                /// @param background The background colour of the box.
                Item(const std::unordered_map<std::wstring, std::wstring>& values, const Colour& foreground, const Colour& background);

                /// Get the value that the item has for the specified key.
                /// @param key The key to search for.
                /// @returns The value for the key.
                std::wstring value(const std::wstring& key) const;

                /// Get the foreground colour.
                /// @returns The foreground text colour.
                Colour foreground() const;

                /// Get the background colour.
                /// @returns The backround colour.
                Colour background() const;

                /// Determines whether two items are equal.
                /// @param other The item to compare.
                /// @returns Whether the items are equal.
                bool operator == (const Item& other) const;
            private:
                std::unordered_map<std::wstring, std::wstring> _values;
                Colour _foreground;
                Colour _background;
            };

            /// A row in the list box.
            class Row final : public StackPanel
            {
            public:
                explicit Row(const Colour& colour, const std::vector<Column>& columns);

                virtual ~Row() = default;

                /// Set the current item for the row.
                /// @param item The item for the row.
                void set_item(const Item& item);

                /// Set the row to have no associated item.
                void clear_item();

                /// Get the item for this row.
                const std::optional<Item>& item() const;

                /// Event raised when a row is clicked.
                Event<Item> on_click;

                void set_highlighted(bool value);

                virtual void mouse_enter() override;
                virtual void mouse_leave() override;
            private:
                void update_row_colour();

                std::optional<Item> _item;
                std::vector<Column> _columns;
                bool _highlighted{ false };
                bool _hovered{ false };
            };

            /// Create a new Listbox.
            /// @param size The size of the listbox.
            /// @param background_colour The background colour for the list box.
            Listbox(const Size& size, const Colour& background_colour);

            /// Create a new Listbox.
            /// @param position The position of the listbox.
            /// @param size The size of the listbox.
            /// @param background_colour The background colour for the list box.
            Listbox(const Point& position, const Size& size, const Colour& background_colour);

            /// Destructor for Listbox.
            virtual ~Listbox();

            /// Set the columns that will be used for sorting and filtering items.
            /// @param columns The column names.
            void set_columns(const std::vector<Column>& columns);

            /// Set whether the user can sort the listbox items.
            void set_enable_sorting(bool value);

            /// Set the items for the list box.
            /// @param items The items to add to the list box.
            void set_items(const std::vector<Item>& items);

            /// Set whether to show a scrollbar.
            /// @param value Whether to show the scrollbar.
            void set_show_scrollbar(bool value);

            /// Set whether to show column headers.
            /// @param value Whether to show column headers.
            void set_show_headers(bool value);

            /// Set whether to highlight rows.
            /// @param value Whether to highlight rows.
            void set_show_highlight(bool value);

            /// Set the selected item. This will not raise the on_item_selected event.
            /// @param item The selected item.
            /// @returns Whether the item was selected.
            bool set_selected_item(const Item& item);

            /// Event raised when an item is selected
            Event<Item> on_item_selected;

            /// Event raised when the delete key is pressed.
            Event<> on_delete;
        protected:
            virtual bool scroll(int delta) override;
            virtual bool key_down(uint16_t key) override;
        private:
            /// Generate all child UI elements.
            void generate_ui();
            /// Generate the header element.
            void generate_headers();
            /// Create the row UI elements to be populated.
            void generate_rows();
            /// Populate the row UI elements with the values from the current listbox item sort.
            void populate_rows();
            /// Sort the items according to the current sort method.
            void sort_items();
            /// Select the given item and scroll to make it visible.
            void select_item(const Item& item, bool raise_event = true);
            /// Scroll the listbox so that the specified item index is visible.
            void scroll_to(uint32_t item);
            /// Scroll the listbox so that the specified item is visible.
            void scroll_to_show(const Item& item);

            void highlight_item();

            std::vector<Column> _columns;
            std::vector<Item> _items;
            StackPanel* _headers_element;
            StackPanel* _rows_container;
            StackPanel* _rows_element;
            Scrollbar* _rows_scrollbar;
            uint32_t _current_top{ 0u };
            Column _current_sort;
            bool _current_sort_direction;
            bool _show_scrollbar{ true };
            bool _show_headers{ true };
            bool _show_highlight{ true };
            bool _enable_sorting{ true };
            std::optional<Item> _selected_item;
            uint32_t _fully_visible_rows{ 0u };
        };
    }
}
