module;

#include <gmock/gmock.h>

export module trview.app:MockPlugins;

import :IPlugins;

namespace trview
{
    namespace mocks
    {
        export struct MockPlugins : public IPlugins
        {
            MockPlugins();
            virtual ~MockPlugins();
            MOCK_METHOD(std::vector<std::weak_ptr<IPlugin>>, plugins, (), (const, override));
            MOCK_METHOD(void, initialise, (IApplication*), (override));
            MOCK_METHOD(void, render_ui, (), (override));
            MOCK_METHOD(void, reload, (), (override));
        };
    }
}
