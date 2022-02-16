#include "TestImGuiId.h"

namespace trview
{
    namespace tests
    {
        TestImGuiId TestImGuiId::push_popup(const std::string& name)
        {
            auto context = ImGui::GetCurrentContext();
            std::string popup_name;

            for (const auto& window : context->Windows)
            {
                if (window->Name == "Debug##Default")
                {
                    auto id = window->GetID(name.c_str());
                    std::stringstream stream;
                    stream << "##Popup_" << std::hex << std::setfill('0') << std::setw(8) << id;
                    popup_name = stream.str();
                }
            }

            for (const auto& window : context->Windows)
            {
                if (window->Name == popup_name)
                {
                    return TestImGuiId(window);
                }
            }

            return *this;
        }

        TestImGuiId TestImGuiId::push_child(const std::string& name)
        {
            auto context = ImGui::GetCurrentContext();
            auto id = _window->GetID(name.c_str());

            std::stringstream stream;
            stream << _window->Name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << id;
            auto child_name = stream.str();

            for (const auto& window : context->Windows)
            {
                if (window->Name == child_name)
                {
                    return TestImGuiId(window);
                }
            }
            throw 0;
        }

        TestImGuiId TestImGuiId::push_override(const std::string& name)
        {
            auto id = _window->GetID(name.c_str());
            _window->IDStack.push_back(id);
            return *this;
        }

        TestImGuiId TestImGuiId::push(const std::string& name)
        {
            auto new_id = _window->GetID(name.c_str());
            std::stringstream stream;
            stream << _window->Name << '/' << name << '_' << std::hex << std::uppercase << std::setfill('0') << std::setw(8) << new_id;
            _name = stream.str();
            _window->IDStack.push_back(new_id);
            return *this;
        }

        TestImGuiId::TestImGuiId(ImGuiWindow* window)
            : _window(window), _id(window->ID), _root_window(window)
        {
            _name = window->Name;
        }

        TestImGuiId TestImGuiId::id(const std::string& name)
        {
            _id = _window->GetID(name.c_str());
            while (_window->IDStack.size() > 1)
            {
                _window->IDStack.pop_back();
            }
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

        ImGuiWindow* TestImGuiId::root() const
        {
            return _root_window;
        }
    }
}
