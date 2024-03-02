#include "pch.h"
#include <trview.app/Windows/ItemsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
// #include <trview.app/Mocks/Elements/ITrigger.h>
// #include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/IItem.h>

#include <ranges>

using namespace testing;
using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<ItemsWindow> build()
            {
                return std::make_unique<ItemsWindow>(clipboard);
            }

            test_module& with_clipboard(const std::shared_ptr<IClipboard>& clipboard)
            {
                this->clipboard = clipboard;
                return *this;
            }
        };

        return test_module{};
    }

    struct ItemsWindowContext
    {
        std::unique_ptr<ItemsWindow> ptr;
        std::vector<std::shared_ptr<IItem>> items;
    };

    void render(ItemsWindowContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_items_window_tests(ImGuiTestEngine* engine)
{
    test<ItemsWindowContext>(engine, "Items Window", "Add to Route Event Raised",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().build();;

            std::shared_ptr<IItem> raised_item;
            auto token = context.ptr->on_add_to_route += [&raised_item](const auto& item) { raised_item = item.lock(); };

            context.items =
            {
                mock_shared<MockItem>()->with_number(0),
                mock_shared<MockItem>()->with_number(1)
            };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_selected_item(context.items[1]);

            ctx->ItemClick("Items 0/**/Add to Route");

            IM_CHECK_NE(raised_item, nullptr);
            IM_CHECK_EQ(raised_item, context.items[1]);
        });

    test<ItemsWindowContext>(engine, "Items Window", "Click Stat Copies",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<ItemsWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto clipboard = mock_shared<MockClipboard>();
            EXPECT_CALL(*clipboard, write).Times(1);

            auto& context = ctx->GetVars<ItemsWindowContext>();
            context.ptr = register_test_module().with_clipboard(clipboard).build();

            context.items =  { mock_shared<MockItem>() };
            context.ptr->set_items({ std::from_range, context.items });
            context.ptr->set_selected_item(context.items[0]);

            ctx->ItemClick("Items 0/**/##itemstats/Position");
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(clipboard.get()), true);
        });
}
