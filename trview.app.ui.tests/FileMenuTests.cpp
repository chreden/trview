#include "pch.h"
#include "FileMenuTests.h"

#include <trview.app/Menus/FileMenu.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>

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
            std::shared_ptr<MockDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };
            FileMenu::LevelNameSource level_name_source{ [](auto&&...) { return std::nullopt; } };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };
            FileMenu::Mode mode{ FileMenu::Mode::Main };

            std::shared_ptr<FileMenu> build()
            {
                return std::make_shared<FileMenu>(dialogs, files, level_name_source, mode, messaging);
            }
        };

        return test_module{};
    }

    struct FileMenuContext
    {
        std::shared_ptr<FileMenu> ptr;
    };

    void render(FileMenuContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_file_menu_tests(ImGuiTestEngine* engine)
{
    test<FileMenuContext>(engine, "File Menu", "Some Test",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<FileMenuContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<FileMenuContext>();
            context.ptr = register_test_module().build();
            ctx->Yield();
        });
}