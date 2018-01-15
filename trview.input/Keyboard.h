#pragma once

#include <cstdint>
#include <vector>
#include <functional>

namespace trview
{
    namespace input
    {
        class Keyboard
        {
        public:
            void on_char(uint16_t character);
            void on_key_down(uint16_t key);
            void on_key_up(uint16_t key);
            void register_key_down(const std::function<void(uint16_t)>& key);
            void register_key_up(const std::function<void(uint16_t)>& key);
            void register_char(const std::function<void(uint16_t)>& key);
            bool control() const;
            void set_control(bool value);
        private:
            std::vector<std::function<void(uint16_t)>> _key_down_listeners;
            std::vector<std::function<void(uint16_t)>> _key_up_listeners;
            std::vector<std::function<void(uint16_t)>> _char_listeners;
            bool _control{ false };
        };
    }
}
