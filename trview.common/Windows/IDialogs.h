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

        struct FileResult
        {
            std::string filename;
            int filter_index;
        };

        struct FileFilter
        {
            std::wstring name;
            std::vector<std::wstring> file_types;
        };

        virtual ~IDialogs() = 0;
        /// <summary>
        /// Show a modal message box.
        /// </summary>
        /// <param name="message">The message of the message box.</param>
        /// <param name="title">The title of the message box.</param>
        /// <param name="buttons">The buttons to show on the message box.</param>
        /// <returns>True if the positive result was chosen.</returns>
        virtual bool message_box(const std::wstring& message, const std::wstring& title, Buttons buttons) const = 0;
        /// <summary>
        /// Prompt the user to select a file to open.
        /// </summary>
        /// <param name="title">The title of the dialog box.</param>
        /// <param name="filters">The filters to use.</param>
        /// <param name="flags">The flags for the dialog.</param>
        /// <returns>The open file result if one was selected or an empty optional.</returns>
        virtual std::optional<FileResult> open_file(const std::wstring& title, const std::vector<FileFilter>& filters, uint32_t flags) const = 0;

        virtual std::optional<std::string> open_folder(const std::wstring& title) const = 0;
        /// <summary>
        /// Prompt the user to select a file to save.
        /// </summary>
        /// <param name="title">The title of the dialog box.</param>
        /// <param name="filters">The file filters to apply.</param>
        /// <param name="filter_index">The file filter to select by default.</param>
        /// <returns>The save file result if one was selected or an empty optional.</returns>
        virtual std::optional<FileResult> save_file(const std::wstring& title, const std::vector<FileFilter>& filters, uint32_t filter_index) const = 0;
    };
}
