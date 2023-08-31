#pragma once

#include <exception>
#include <string>

namespace trlevel
{
    struct LevelLoadException final : public std::exception
    {
        std::string message;

        LevelLoadException()
            : message("Unknown level load error")
        {
        }

        LevelLoadException(const std::string& message)
            : message(message)
        {
        }

        const char* what() const override
        {
            return message.c_str();
        }
    };
}