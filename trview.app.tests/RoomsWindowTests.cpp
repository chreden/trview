#include <trview.app/Windows/RoomsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/UI/IMapRenderer.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.common/Mocks/Windows/IClipboard.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            IMapRenderer::Source map_renderer_source{ [](auto&&...) { return mock_unique<MockMapRenderer>(); } };
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<RoomsWindow> build()
            {
                return std::make_unique<RoomsWindow>(map_renderer_source, clipboard);
            }

            test_module& with_map_renderer_source(IMapRenderer::Source map_renderer_source)
            {
                this->map_renderer_source = map_renderer_source;
                return *this;
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(RoomsWindow, SetMapColoursUpdatesMapRenderer)
{
    auto [map_renderer_ptr, map_renderer] = create_mock<MockMapRenderer>();
    EXPECT_CALL(map_renderer, set_settings).Times(1);

    auto window = register_test_module().with_map_renderer_source([&]() { return std::move(map_renderer_ptr); }).build();
    window->set_settings({});
}
