export module trview.app:Toolbar;

import std;

import :IToolbar;
import :IPlugins;

namespace trview
{
    /// Window that contains buttons for speedrun planning tools.
    export class Toolbar final : public IToolbar
    {
    public:
        explicit Toolbar(const std::weak_ptr<IPlugins>& plugins);

        /// Add a new tool to the toolbar.
        /// @param name The name of the tool
        void add_tool(const std::string& name) override;

        void render() override;
    private:
        std::vector<std::string> _tools;
        std::weak_ptr<IPlugins> _plugins;
    };
}
