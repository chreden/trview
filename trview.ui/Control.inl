#pragma once

namespace trview
{
    namespace ui
    {
        template <typename T>
        T* Control::find(const std::string& name) const
        {
            if (name == name())
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
    }
}
