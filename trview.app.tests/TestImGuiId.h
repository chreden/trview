#pragma once

#include <external/imgui/imgui.h>
#include <external/imgui/imgui_internal.h>

namespace trview
{
    namespace tests
    {
        class TestImGuiId
        {
        public:
            TestImGuiId();
            explicit TestImGuiId(const std::string& name);
            TestImGuiId push_popup(const std::string& name);
            TestImGuiId push_child(const std::string& name);
            TestImGuiId push_override(const std::string& name);
            TestImGuiId push(const std::string& name);
            TestImGuiId id(const std::string& name);
            ImGuiID id() const;

            std::string name() const;
            ImGuiID lowest_window() const;
            ImGuiID root() const;
        private:
            ImGuiID GetID(const std::string& name);

            ImVector<ImGuiID> _stack;
            std::string _name;
            ImGuiID _id;
            ImGuiID _root;
            ImGuiID _lowest_window;
        };
    }
}