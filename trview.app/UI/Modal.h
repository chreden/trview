#pragma once

namespace trview
{
    template <typename State>
    class Modal
    {
    public:
        void show(const std::string& id, const State& state);
        void render(const std::function<bool (State&)>& callback);
    private:
        std::string _id;
        State _state;
        std::optional<bool> _open;
        bool _is_open{ false };
    };
}

#include "Modal.hpp"
