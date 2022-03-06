#pragma once

#include <external/imgui/imgui.h>

namespace trview
{
    namespace tests
    {
        class TestImGuiId
        {
        public:
            TestImGuiId();
            explicit TestImGuiId(const std::string& name);
            TestImGuiId push_popup(const std::string& name) const;
            TestImGuiId push_child(const std::string& name) const;
            TestImGuiId push_override(const std::string& name) const;
            TestImGuiId push(const std::string& name) const;
            TestImGuiId id(const std::string& name) const;
            ImGuiID id() const;
            std::string name() const;
            ImGuiID root() const;
            ImGuiID hover() const;
        private:
            ImGuiID GetID(const std::string& name) const;

            ImVector<ImGuiID> _stack;
            std::string _name;
            ImGuiID _id{ 0u };
            ImGuiID _root{ 0u };
            ImGuiID _hover_id{ 0u };
        };
    }
}