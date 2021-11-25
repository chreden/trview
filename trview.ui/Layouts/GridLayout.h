#pragma once

#include "ILayout.h"
#include <trview.common/TokenStore.h>

namespace trview
{
    namespace ui
    {
        class GridLayout final : public ILayout
        {
        public:
            GridLayout(uint32_t columns, uint32_t rows);
            virtual ~GridLayout() = default;
            virtual void bind(Control& control) override;
            uint32_t columns() const;
            uint32_t rows() const;
        private:
            TokenStore _token_store;
            uint32_t _columns{ 1u };
            uint32_t _rows{ 1u };
            uint32_t _cells{ 0u };
        };
    }
}
