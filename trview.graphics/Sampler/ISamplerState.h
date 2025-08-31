#pragma once

namespace trview
{
    namespace graphics
    {
        struct ISamplerState
        {
            enum class AddressMode
            {
                Wrap,
                Clamp
            };

            enum class FilterMode
            {
                Point,
                Linear
            };

            using Source = std::function<std::shared_ptr<ISamplerState>(AddressMode)>;
            virtual ~ISamplerState();
            virtual void apply() = 0;
            virtual void set_filter_mode(FilterMode mode) = 0;
        };
    }
}
