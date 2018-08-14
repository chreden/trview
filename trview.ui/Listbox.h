/// @file Listbox.h
/// @brief UI element that displays a number of rows in a grid with column headers.

#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include <trview.common/TokenStore.h>

#include "StackPanel.h"

namespace trview
{
    namespace ui
    {
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
                /// @parm name The column name. This is displayed as a header.
                Column(Type type, const std::wstring& name);

                /// Get the name of the column. This is displayed as a header.
                /// @returns The name of the column.
                const std::wstring& name() const;

                /// Get the type of the column data.
                /// @returns The data type of the column.
                Type type() const;
            private:
                std::wstring _name;
                Type _type;
            };

            /// An entry in the list box.
            class Item final
            {
            public:
                /// Create an item.
                /// @param values The column index values for the item.
                Item(const std::unordered_map<std::wstring, std::wstring>& values);

                /// Get the value that the item has for the specified key.
                /// @param key The key to search for.
                /// @returns The value for the key.
                std::wstring value(const std::wstring& key) const;
            private:
                std::unordered_map<std::wstring, std::wstring> _values;
            };

            /// Create a new Listbox.
            /// @param position The position of the listbox.
            /// @param size The size of the listbox.
            Listbox(const Point& position, const Size& size);

            /// Destructor for Listbox.
            virtual ~Listbox();

            /// Set the columns that will be used for sorting and filtering items.
            /// @param columns The column names.
            void set_columns(const std::vector<Column>& columns);

            /// Set the items for the list box.
            /// @param items The items to add to the list box.
            void set_items(const std::vector<Item>& items);
        protected:
            virtual bool scroll(int delta) override;
        private:
            /// Create the row UI elements to be populated.
            void generate_rows();
            /// Populate the row UI elements with the values from the current listbox item sort.
            void populate_rows();
            /// Sort the items according to the current sort method.
            void sort_items();

            std::vector<Column> _columns;
            std::vector<Item> _items;
            StackPanel* _headers_element;
            StackPanel* _rows_element;
            int32_t _current_top{ 0 };
            Column _current_sort;
            bool _current_sort_direction;
            TokenStore _token_store;
        };
    }
}
