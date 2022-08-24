#include "TestImGuiId.h"
#include <external/imgui/imgui_internal.h>
#include <format>

namespace trview
{
    namespace tests
    {
        TestImGuiId TestImGuiId::push_popup(const std::string& name) const
        {
            return TestImGuiId(std::format("##Popup_{:0>8x}", TestImGuiId("Debug##Default").id(name).id()));
        }

        TestImGuiId TestImGuiId::push_child(const std::string& name) const
        {
            return TestImGuiId(std::format("{}/{}_{:0>8X}", _name, name, GetID(name.c_str())));
        }

        TestImGuiId TestImGuiId::push_override(const std::string& name) const
        {
            TestImGuiId copy = *this;
            copy._stack.push_back(copy.GetID(name.c_str()));
            return copy;
        }

        TestImGuiId TestImGuiId::push(const std::string& name) const
        {
            TestImGuiId copy = *this;
            auto new_id = copy.GetID(name.c_str());
            copy._name = std::format("{}/{}_{:0>8X}", _name, name, new_id);
            copy._stack.push_back(new_id);
            copy._hover_id = push_child(name).id();
            return copy;
        }

        TestImGuiId::TestImGuiId()
            : _id(0)
        {
        }

        TestImGuiId::TestImGuiId(const std::string& name)
            : _name(name), _id(GetID(name)), _root(_id)
        {
            _stack.push_back(_id);
        }

        TestImGuiId TestImGuiId::id(const std::string& name) const
        {
            TestImGuiId copy = *this;
            copy._id = copy.GetID(name);
            return copy;
        }

        ImGuiID TestImGuiId::id() const
        {
            return _id;
        }

        std::string TestImGuiId::name() const
        {
            return _name;
        }

        ImGuiID TestImGuiId::root() const
        {
            return _root;
        }

        ImGuiID TestImGuiId::GetID(const std::string& name) const
        {
            ImGuiID seed = _stack.empty() ? 0 : _stack.back();
            return ImHashStr(name.c_str(), name.size(), seed);
        }

        ImGuiID TestImGuiId::hover() const
        {
            return _hover_id;
        }
    }
}
