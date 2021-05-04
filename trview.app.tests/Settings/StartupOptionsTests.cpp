#include <trview.app/Settings/StartupOptions.h>

using namespace trview;

TEST(StartupOptions, FilenameLoadedFromCommandLine)
{
    StartupOptions startup_options{ L"trview.exe c:\\test.tr2" };
    ASSERT_EQ(startup_options.filename(), "c:\\test.tr2");
}

TEST(StartupOptions, FlagsWithNoFile)
{
    StartupOptions startup_options{ L"trview.exe -ff1 -ff2" };
    ASSERT_EQ(startup_options.filename(), "");
    ASSERT_EQ(startup_options.feature("ff1"), true);
    ASSERT_EQ(startup_options.feature("ff2"), true);
}

TEST(StartupOptions, FlagsWithFile)
{
    StartupOptions startup_options{ L"trview.exe c:\\test.tr2 -ff1 -ff2" };
    ASSERT_EQ(startup_options.filename(), "c:\\test.tr2");
    ASSERT_EQ(startup_options.feature("ff1"), true);
    ASSERT_EQ(startup_options.feature("ff2"), true);
}
