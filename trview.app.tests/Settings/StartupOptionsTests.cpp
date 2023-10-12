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
    StartupOptions startup_options{ L"trview.exe -ff1 -ff2 c:\\test.tr2" };
    ASSERT_EQ(startup_options.filename(), "c:\\test.tr2");
    ASSERT_EQ(startup_options.feature("ff1"), true);
    ASSERT_EQ(startup_options.feature("ff2"), true);
}

TEST(StartupOptions, MissingFlagIsFalse)
{
    StartupOptions startup_options{ L"trview.exe c:\\test.tr2 -ff2" };
    ASSERT_EQ(startup_options.feature("ff1"), false);
}

TEST(StartupOptions, FilenameWithSpaces)
{
    StartupOptions startup_options{ L"trview.exe C:\\Path with spaces\\test.tr2" };
    ASSERT_EQ(startup_options.filename(), "C:\\Path with spaces\\test.tr2");
}

TEST(StartupOptions, FlagsWithFileAndSpaces)
{
    StartupOptions startup_options{ L"trview.exe -ff1 -ff2 c:\\test 2.tr2" };
    ASSERT_EQ(startup_options.filename(), "c:\\test 2.tr2");
    ASSERT_EQ(startup_options.feature("ff1"), true);
    ASSERT_EQ(startup_options.feature("ff2"), true);
}