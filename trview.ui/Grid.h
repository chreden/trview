#pragma once

namespace trview
{
    namespace ui
    {
        class Grid final
        {
        public:
            enum class InsertOrder
            {
                Column,
                Row
            };

            explicit Grid(InsertOrder insert_order);
        private:
            InsertOrder _insert_order;
        };
    }
}
