#pragma once

#include <string>
#include "../Window.h"

namespace trview
{
    struct IDialogs
    {
        enum class Buttons
        {
            OK_Cancel
        };

        virtual ~IDialogs() = 0;
        /// <summary>
        /// Show a modal message box.
        /// </summary>
        /// <param name="window">The window that owns the message box.</param>
        /// <param name="title">The title of the message box.</param>
        /// <param name="message">The message of the message box.</param>
        /// <param name="buttons">The buttons to show on the message box.</param>
        /// <returns>True if the positive result was chosen.</returns>
        virtual bool message_box(const Window& window, const std::wstring& title, const std::wstring& message, Buttons buttons) = 0;
    };
}
