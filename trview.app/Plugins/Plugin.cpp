#include "Plugin.h"
#include <algorithm>
#include <format>
#include "../Geometry/PickResult.h"
#include "../Elements/IRoom.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(std::unique_ptr<ILua> lua, const std::string& name, const std::string& author, const std::string& description)
        : _lua(std::move(lua)), _name(name), _author(author), _description(description), _built_in(true)
    {
        register_print();
    }

    Plugin::Plugin(const std::shared_ptr<IFiles>& files,
        std::unique_ptr<ILua> lua,
        const std::string& path)
        : _lua(std::move(lua)), _path(path), _files(files)
    {
        _lua->set_directory(_path);
        register_print();
        load();
    }

    void Plugin::register_print()
    {
        _token_store += _lua->on_print += [&](const std::string& message) { add_message(message); };
    }

    bool Plugin::built_in() const
    {
        return _built_in;
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

    bool Plugin::enabled() const
    {
        return _enabled;
    }

    void Plugin::initialise(IApplication* application)
    {
        _application = application;
        _lua->initialise(application);
        set_package_path();
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
        initialise(_application);
    }

    void Plugin::load()
    {
        if (_files)
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
    }

    void Plugin::load_script()
    {
        if (!_enabled)
        {
            return;
        }

        if (!_script.empty())
        {
            _lua->do_file(_script);
            _script_loaded = true;
        }
    }

    namespace
    {
        std::weak_ptr<IRoom> room_from_pick(const PickResult& pick)
        {
            switch (pick.type)
            {
            case PickResult::Type::Room:
            {
                return pick.room;
            }
            case PickResult::Type::Entity:
            {
                if (auto item = pick.item.lock())
                {
                    return item->room();
                }
                break;
            }
            case PickResult::Type::Trigger:
            {
                if (const auto trigger = pick.trigger.lock())
                {
                    return trigger->room();
                }
                break;
            }
            }
            return {};
        }
    }


    void Plugin::render_context_menu(const PickResult& pick_result)
    {
        if (!_enabled)
        {
            return;
        }

        if (auto room = room_from_pick(pick_result).lock())
        {
            const float x = pick_result.position.x * trlevel::Scale;
            const float y = pick_result.position.y * trlevel::Scale;
            const float z = pick_result.position.z * trlevel::Scale;
            _lua->execute(std::format("if render_context_menu ~= nil then local pick = {{ room = {}, x = {}, y = {}, z = {} }} render_context_menu(pick) end", room->number(), x, y, z));
        }
    }

    void Plugin::render_toolbar()
    {
        if (!_enabled)
        {
            return;
        }

        _lua->execute("if render_toolbar ~= nil then render_toolbar() end");
    }

    void Plugin::render_ui()
    {
        if (!_enabled)
        {
            return;
        }

        _lua->execute("if render_ui ~= nil then render_ui() end");
    }

    void Plugin::set_package_path()
    {
        std::string escaped = _path;
        std::ranges::replace(escaped, '\\', '/');
        _lua->execute(std::format("package.path = \"{}/?.lua\"", escaped));
    }

    void Plugin::set_enabled(bool value)
    {
        if (_built_in)
        {
            return;
        }

        _enabled = value;
        if (!_script_loaded)
        {
            load_script();
        }

        _lua->execute(std::format("if set_enabled ~= nil then set_enabled({}) end", value));
    }
}
