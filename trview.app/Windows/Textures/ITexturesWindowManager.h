#pragma once

#include "ITexturesWindow.h"

namespace trview
{
    struct ITexturesWindowManager
    {
        virtual ~ITexturesWindowManager() = 0;
        virtual std::weak_ptr<ITexturesWindow> create_window() = 0;
        virtual void render() = 0;
    };
}
