/// @file Scrollbar.h
/// @brief Used to scroll across a range of items or values.

#pragma once

#include "Window.h"

namespace trview
{
    namespace ui
    {
        /// Used to scroll across a range of items or values.
        class Scrollbar final : public Window
        {
        public:
            /// Create a new scrollbar.
            /// @param position The position for the scrollbar.
            /// @param size The size of the new scrollbar.
            Scrollbar(const Point& position, const Size& size);

            /// Destructor for scrollbar.
            virtual ~Scrollbar() = default;

            /// Set the viewable range that the scrollbar is representing. This will
            /// change the position and size of the scroller blob.
            /// @param range_start The first item that can be seen.
            /// @param range_end The last item that can be seen.
            /// @param maximum The number of 'items' being scrolled through.
            void set_range(float range_start, float range_end, float maximum);

            /// Event raised when the user has moved the blob in the scrollbar.
            Event<float> on_scroll;
        protected:
            virtual bool clicked(Point position) override;
        private:
            Window* _blob;
        };
    }
}
