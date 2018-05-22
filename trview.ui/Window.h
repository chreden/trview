#pragma once

#include "Control.h"
#include "Colour.h"
#include "Point.h"

namespace trview
{
    namespace ui
    {
        class Window : public Control
        {
        public:
            explicit Window(Point position, Size size, Colour background_colour);

            virtual ~Window() = default;

            // The colour that the area defined by this window will be filled with. 
            // Can be set with set_background_colour.
            Colour  background_colour() const;

            // Sets the colour that the area defined by the window will be filled with.
            // Setting this property will prompt the control to redraw at the next opportunity.
            // Get the current value with get_background_colour.
            void    set_background_colour(Colour colour);
        private:
            Colour _background_colour;
        };
    }
}