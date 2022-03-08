#include <trview.app/Windows/Floordata/FloordataWindow.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<FloordataWindow> build()
            {
                return std::make_unique<FloordataWindow>();
            }
        };
        return test_module{};
    }
}

