/// @file Toolbar.h
/// @brief Window that contains buttons for speedrun planning tools.

#pragma once

#include <string>

namespace trview
{
    /// Window that contains buttons for speedrun planning tools.
    class Toolbar final
    {
    public:
        /// Add a new tool to the toolbar.
        /// @param name The name of the tool
        void add_tool(const std::string& name);

        void render();

        /// Event raised when a tool button is clicked.
        /// @remarks The name of the tool is passed as a parameter.
        Event<const std::string&> on_tool_clicked;
    private:
        std::vector<std::string> _tools;
    };
}
