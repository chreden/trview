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

            using Source = std::function<std::shared_ptr<ISamplerState>(AddressMode)>;
            virtual ~ISamplerState();
            virtual void apply() = 0;
        };
    }
}
