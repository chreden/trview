module;

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>
#include <external/imgui/misc/cpp/imgui_stdlib.h>

export module trview.app:Modal;

import std;

namespace trview
{
    export template <typename State>
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
