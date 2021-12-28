#include "Application.h"

namespace trview
{
    void Application::register_lua()
    {
        lua_registry.trview_openrecent = [this](int index)
        {
            auto i = static_cast<std::size_t> (index);
            const auto& config = _settings;
            const auto recent_files = config.recent_files;
            if (i > 0 && i <= recent_files.size())
            {
                open(*std::next(recent_files.begin(), i - 1));
            }
        };

        lua_registry.trview_open = [this](const std::string& path)
        {
            open ( path );
        };
    }
}
