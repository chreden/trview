#include "Plugin.h"

namespace trview
{
    IPlugin::~IPlugin()
    {
    }

    Plugin::Plugin(const std::shared_ptr<IFiles>& files, const std::string& path)
    {
        files;
        path;
    }
}
