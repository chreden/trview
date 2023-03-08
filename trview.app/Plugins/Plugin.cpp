#include "Plugin.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(std::unique_ptr<ILua> lua, const std::string& name, const std::string& author, const std::string& description)
        : _lua(std::move(lua)), _name(name), _author(author), _description(description)
    {
        register_print();
    }

    Plugin::Plugin(const std::shared_ptr<IFiles>& files,
        std::unique_ptr<ILua> lua,
        const std::string& path)
        : _lua(std::move(lua)), _path(path), _files(files)
    {
        register_print();
        load();
    }

    void Plugin::register_print()
    {
        _token_store += _lua->on_print += [&](const std::string& message) { add_message(message); };
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
        load_script();
    }

    std::string Plugin::messages() const
    {
        return _messages;
    }

    void Plugin::execute(const std::string& command)
    {
        _lua->execute(command);
    }

    void Plugin::add_message(const std::string& message)
    {
        if (!_messages.empty())
        {
            _messages += '\n';
        }
        _messages += message;
        on_message(message);
    }

    void Plugin::do_file(const std::string& path)
    {
        _lua->do_file(path);
    }

    void Plugin::clear_messages()
    {
        _messages.clear();
    }

    void Plugin::reload()
    {
        load();
        load_script();
    }

    void Plugin::load()
    {
        auto manifest = _files->load_file(_path + "\\manifest.json");
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
        _script = _path + "\\plugin.lua";
    }

    void Plugin::load_script()
    {
        if (!_script.empty())
        {
            _lua->do_file(_script);
        }
    }
}
