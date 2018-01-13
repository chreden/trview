#include "Keyboard.h"
#include <Windows.h>
#include <algorithm>

namespace trview
{
    namespace input
    {
        void Keyboard::on_char(uint16_t character)
        {
            std::for_each(_char_listeners.begin(), _char_listeners.end(), [=](auto f) { f(character); });
        }

        void Keyboard::on_key_down(uint16_t key)
        {
            std::for_each(_key_down_listeners.begin(), _key_down_listeners.end(), [=](auto f) { f(key); });
        }

        void Keyboard::on_key_up(uint16_t key)
        {
            std::for_each(_key_up_listeners.begin(), _key_up_listeners.end(), [=](auto f) { f(key); });
        }

        void Keyboard::register_key_down(const std::function<void(unsigned short)>& key)
        {
            _key_down_listeners.push_back(key);
        }

        void Keyboard::register_key_up(const std::function<void(unsigned short)>& key)
        {
            _key_up_listeners.push_back(key);
        }

        void Keyboard::register_char(const std::function<void(unsigned short)>& key)
        {
            _char_listeners.push_back(key);
        }
    }
}
