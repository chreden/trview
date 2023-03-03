#include "Plugin.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(const std::shared_ptr<IFiles>& files,
        std::unique_ptr<ILua> lua,
        const std::string& path)
        : _lua(std::move(lua))
    {
        auto manifest = files->load_file(path + "\\manifest.json");
        if (manifest)
        {
            try
            {
                auto manifest_json = nlohmann::json::parse(manifest.value().begin(), manifest.value().end());

                read_attribute(manifest_json, _name, "name");
                read_attribute(manifest_json, _author, "author");
                read_attribute(manifest_json, _description, "description");
            }
            catch (...)
            {

            }
        }
    }

    std::string Plugin::name() const
    {
        return _name;
    }

    std::string Plugin::author() const
    {
        return _author;
    }

    std::string Plugin::description() const
    {
        return _description;
    }

    void Plugin::initialise(IApplication* application)
    {
        _lua->initialise(application);
    }
}
