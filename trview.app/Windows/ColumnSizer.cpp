#include "ColumnSizer.h"

namespace trview
{
    void ColumnSizer::measure(const std::string& value, uint32_t index)
    {
        if (ImGui::GetCurrentContext() == nullptr)
        {
            return;
        }

        if (index >= _sizes.size())
        {
            _sizes.resize(index + 1, 0.0f);
        }
        _sizes[index] = std::max(_sizes[index], ImGui::CalcTextSize(value.c_str()).x);
    }

    void ColumnSizer::reset()
    {
        _sizes.clear();
    }

    float ColumnSizer::size() const
    {
        float total = 0.0f;
        for (std::size_t i = 0; i < _sizes.size(); ++i)
        {
            total += size(static_cast<uint32_t>(i));
        }
        return total;
    }

    float ColumnSizer::size(uint32_t index) const
    {
        return index < _sizes.size() ? _sizes[index] : 0.0f;
    }
}
