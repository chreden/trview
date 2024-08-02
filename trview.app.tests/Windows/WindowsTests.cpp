#include <trview.app/Windows/Windows.h>

#include <trview.app/Mocks/Windows/IPluginsWindowManager.h>
#include <trview.app/Mocks/Windows/IStaticsWindowManager.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<IPluginsWindowManager> plugins{ mock_unique<MockPluginsWindowManager>() };
            std::unique_ptr<IStaticsWindowManager> statics{ mock_unique<MockStaticsWindowManager>() };

            std::unique_ptr<trview::Windows> build()
            {
                return std::make_unique<trview::Windows>(std::move(plugins), std::move(statics));
            }
        };
        return test_module{};
    }
}

TEST(Windows, Statics)
{
}

TEST(Windows, Plugins)
{
}