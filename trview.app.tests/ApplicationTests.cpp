#include <trview.app/Application.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>

using namespace trview;
using namespace trview::tests;

TEST(Application, ChecksForUpdates)
{
    auto [update_checker_ptr, update_checker] = create_mock<mocks::MockUpdateChecker>();
    EXPECT_CALL(update_checker, check_for_updates).Times(1);

    CoInitialize(nullptr);
    Application application(create_test_window(L"ApplicationTests"), std::move(update_checker_ptr), std::wstring());
}
