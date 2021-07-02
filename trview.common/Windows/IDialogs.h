#pragma once

#include <optional>
#include <string>
#include <vector>
#include "../Window.h"

namespace trview
{
    struct IDialogs
    {
        enum class Buttons
        {
            OK,
            OK_Cancel,
            Yes_No
        };

        virtual ~IDialogs() = 0;
        /// <summary>
        /// Show a modal message box.
        /// </summary>
        /// <param name="window">The window that owns the message box.</param>
        /// <param name="message">The message of the message box.</param>
        /// <param name="title">The title of the message box.</param>
        /// <param name="buttons">The buttons to show on the message box.</param>
        /// <returns>True if the positive result was chosen.</returns>
        virtual bool message_box(const Window& window, const std::wstring& message, const std::wstring& title, Buttons buttons) const = 0;
        /// <summary>
        /// Prompt the user to select a file to open.
        /// </summary>
        /// <param name="title">The title of the dialog box.</param>
        /// <param name="filter">The display text for the file filters.</param>
        /// <param name="file_types">The file types to allow.</param>
        /// <param name="flags">The flags for the dialog.</param>
        /// <returns>The filename if one was selected or an empty optional.</returns>
        virtual std::optional<std::string> open_file(const std::wstring& title, const std::wstring& filter, const std::vector<std::wstring>& file_types, uint32_t flags) const = 0;
        /// <summary>
        /// Prompt the user to select a file to save.
        /// </summary>
        /// <param name="title">The title of the dialog box.</param>
        /// <param name="filter">The display text for the file filters.</param>
        /// <param name="file_types">The file types to allow.</param>
        /// <returns>The filename if one was selected or an empty optional.</returns>
        virtual std::optional<std::string> save_file(const std::wstring& title, const std::wstring& filter, const std::vector<std::wstring>& file_types) const = 0;
    };
}
