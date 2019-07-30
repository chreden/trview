#pragma once

namespace trview
{
    namespace ui
    {
        template <typename T>
        T* Control::find(const std::string& name) const
        {
            if (name == this->name())
            {
                return dynamic_cast<T*>(this);
            }

            for (const auto& child : _child_elements)
            {
                T* result = child->find(name);
                if (result)
                {
                    return result;
                }
            }

            return nullptr;
        }

        template <typename T>
        T* Control::find(const std::string& name)
        {
            if (name == this->name())
            {
                return dynamic_cast<T*>(this);
            }

            for (const auto& child : _child_elements)
            {
                T* result = child->find<T>(name);
                if (result)
                {
                    return result;
                }
            }

            return nullptr;
        }

        template <typename T>
        T* Control::add_child(std::unique_ptr<T>&& child_element)
        {
            static_assert(std::is_base_of<Control, T>::value, "Element must be derived from Control");

            child_element->_parent = this;
            T* element = static_cast<T*>(child_element.get());
            _child_elements.push_back(std::move(child_element));

            inner_add_child(element);
            on_invalidate();
            on_hierarchy_changed();
            on_add_child(static_cast<Control*>(element));

            return element;
        }
    }
}
