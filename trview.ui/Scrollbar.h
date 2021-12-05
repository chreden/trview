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
            struct Names
            {
                static const std::string blob;
            };

            /// Create a new scrollbar.
            /// @param size The size of the new scrollbar.
            /// @param background_colour The background colour of the scrollbar.
            Scrollbar(const Size& size, const Colour& background_colour);

            /// Create a new scrollbar.
            /// @param position The position for the scrollbar.
            /// @param size The size of the new scrollbar.
            /// @param background_colour The background colour of the scrollbar.
            Scrollbar(const Point& position, const Size& size, const Colour& background_colour);

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

            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool move(Point position) override;
            virtual bool clicked(Point position) override;
        private:
            Window* _blob;
        };
    }
}
