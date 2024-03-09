#include "pch.h"
#include "LogWindowTests.h"
#include <trview.app/Windows/Log/LogWindow.h>
#include <trview.common/Mocks/Logs/ILog.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;
using testing::Return;
using testing::An;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<ILog> log{ mock_shared<MockLog>() };
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };

            std::unique_ptr<LogWindow> build()
            {
                return std::make_unique<LogWindow>(log, dialogs, files);
            }

            test_module& with_logs(const std::shared_ptr<ILog>& log)
            {
                this->log = log;
                return *this;
            }

            test_module& with_dialogs(const std::shared_ptr<IDialogs>& dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }
        };

        return test_module{};
    }

    struct LogWindowContext
    {
        std::unique_ptr<LogWindow> ptr;
    };

    void render(LogWindowContext& context)
    {
        if (context.ptr)
        {
            context.ptr->render();
        }
    }
}

void register_log_window_tests(ImGuiTestEngine* engine)
{
    test<LogWindowContext>(engine, "Log Window", "Save Button Exports Data",
        [](ImGuiTestContext* ctx) { render(ctx->GetVars<LogWindowContext>()); },
        [](ImGuiTestContext* ctx)
        {
            auto& context = ctx->GetVars<LogWindowContext>();
            auto dialogs = mock_shared<MockDialogs>();
            auto files = mock_shared<MockFiles>();
            context.ptr = register_test_module().with_dialogs(dialogs).with_files(files).build();

            EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(testing::Return<IDialogs::FileResult>({ "fn", 0 }));
            EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::string&>())).Times(1);

            ctx->ItemClick("/Log 0/topics/All/Save");

            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(dialogs.get()), true);
            IM_CHECK_EQ(Mock::VerifyAndClearExpectations(files.get()), true);
        });
}