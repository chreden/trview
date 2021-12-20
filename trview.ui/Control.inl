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
        std::vector<const T*> Control::find() const
        {
            std::vector<const T*> results;
            T* as_t = dynamic_cast<const T*>(this);
            if (as_t)
            {
                results.push_back(as_t);
            }

            for (const auto& child : _child_elements)
            {
                auto child_results = child->find<T>();
                results.insert(results.end(), child_results.begin(), child_results.end());
            }
            return results;
        }

        template <typename T>
        std::vector<T*> Control::find()
        {
            std::vector<T*> results;
            T* as_t = dynamic_cast<T*>(this);
            if (as_t)
            {
                results.push_back(as_t);
            }

            for (auto& child : _child_elements)
            {
                auto child_results = child->find<T>();
                results.insert(results.end(), child_results.begin(), child_results.end());
            }
            return results;
        }

        template <typename T>
        T* Control::add_child(std::unique_ptr<T>&& child_element)
        {
            static_assert(std::is_base_of<Control, T>::value, "Element must be derived from Control");

            child_element->set_parent(this);
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
