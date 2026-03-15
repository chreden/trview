#pragma once

namespace trview
{
    template <typename State>
    class Modal
    {
    public:
        void show(const State& state);
        void render(const std::function<bool (State&)>& callback);
    private:
        State _state;
        std::optional<bool> _open;
        bool _is_open{ false };
    };
}

#include "Modal.hpp"
