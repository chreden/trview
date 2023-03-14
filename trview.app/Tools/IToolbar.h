#pragma once

namespace trview
{
    struct IToolbar
    {
        virtual ~IToolbar() = 0;
        virtual void add_tool(const std::string& name) = 0;
        virtual void render() = 0;

        /// Event raised when a tool button is clicked.
        /// @remarks The name of the tool is passed as a parameter.
        Event<const std::string&> on_tool_clicked;
    };
}
