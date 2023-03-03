#include "Plugin.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(const std::shared_ptr<IFiles>& files,
        std::unique_ptr<ILua> lua,
        const std::string& path)
        : _lua(std::move(lua)), _path(path)
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

        _script = path + "\\plugin.lua";
        _token_store += _lua->on_print += [&](const std::string& message)
        {
            _messages.push_back(message);
        };
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

    std::string Plugin::path() const
    {
        return _path;
    }

    void Plugin::initialise(IApplication* application)
    {
        _lua->initialise(application);
        _lua->do_file(_script);
    }
}
