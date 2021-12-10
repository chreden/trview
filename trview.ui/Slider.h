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
            struct Names
            {
                static const std::string blob;
            };

            Slider(Point position, Size size);
            explicit Slider(const Size& size);
            virtual ~Slider() = default;

            Event<float> on_value_changed;

            float value() const;
            void set_value(float value);
            virtual bool mouse_down(const Point& position) override;
            virtual bool mouse_up(const Point& position) override;
            virtual bool clicked(Point position) override;
            virtual bool move(Point position) override;
        private:
            void set_blob_position(Point position, bool raise_event = false);
            void set_blob_position(float percentage, bool raise_event = false);

            float _value{ 0.0f };
            ui::Control* _blob;
        };
    }
}