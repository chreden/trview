#pragma once

#include "Window.h"
#include <trview.common/Event.h>

namespace trview
{
    namespace ui
    {
        class Slider : public Window
        {
        public:
            Slider(Point position, Size size);
            virtual ~Slider() = default;

            Event<float> on_value_changed;

            void set_value(float value);
            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool clicked(Point position) override;
            virtual bool move(Point position) override;
        private:
            void set_blob_position(Point position);
            void set_blob_position(float percentage);

            float _value{ 0.5f };
            ui::Control* _blob;
        };
    }
}