#include "TestImGuiId.h"

namespace trview
{
    namespace tests
    {
        TestImGuiId TestImGuiId::push_popup(const std::string& name)
        {
            auto context = ImGui::GetCurrentContext();
            std::string popup_name;

            std::stringstream stream;
            stream << "##Popup_" << std::hex << std::setfill('0') << std::setw(8) << TestImGuiId("Debug##Default").id(name).id();
            popup_name = stream.str();

            return TestImGuiId(popup_name);
        }

        TestImGuiId TestImGuiId::push_child(const std::string& name)
        {
            auto context = ImGui::GetCurrentContext();
            auto id = GetID(name.c_str());

            std::stringstream stream;
            stream << _name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << id;
            auto child_name = stream.str();

            return TestImGuiId(child_name);
        }

        TestImGuiId TestImGuiId::push_override(const std::string& name)
        {
            auto id = GetID(name.c_str());
            _stack.push_back(id);
            return *this;
        }

        TestImGuiId TestImGuiId::push(const std::string& name)
        {
            auto new_id = GetID(name.c_str());
            std::stringstream stream;
            stream << _name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << new_id;
            _name = stream.str();
            _stack.push_back(new_id);
            return *this;
        }

        TestImGuiId::TestImGuiId()
            : _id(0)
        {
        }

        TestImGuiId::TestImGuiId(const std::string& name)
            : _name(name), _id(GetID(name))
        {
            _stack.push_back(_id);
            _lowest_window = _id;
        }

        TestImGuiId TestImGuiId::id(const std::string& name)
        {
            _id = GetID(name);
            return *this;
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

        ImGuiID TestImGuiId::GetID(const std::string& name)
        {
            ImGuiID seed = _stack.empty() ? 0 : _stack.back();
            return ImHashStr(name.c_str(), name.size(), seed);
        }

        ImGuiID TestImGuiId::lowest_window() const
        {
            return _lowest_window;
        }
    }
}
