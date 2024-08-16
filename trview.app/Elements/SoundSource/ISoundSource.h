#pragma once

#include <functional>
#include <memory>

namespace trview
{
    struct ISoundSource
    {
        using Source = std::function<std::shared_ptr<ISoundSource>(uint32_t)>;
        virtual ~ISoundSource() = 0;
        virtual uint32_t number() const = 0;
    };
}
