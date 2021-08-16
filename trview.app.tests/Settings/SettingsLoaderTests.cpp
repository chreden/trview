#include <trview.app/Settings/SettingsLoader.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using testing::Return;
using testing::A;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IFiles> files{ std::make_shared<MockFiles>() };

            std::unique_ptr<SettingsLoader> build()
            {
                return std::make_unique<SettingsLoader>(files);
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }
        };

        return test_module{};
    };
}

TEST(SettingsLoader, FileNotFound)
{
    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
    EXPECT_CALL(*files, load_file("appdata\\trview\\settings.txt")).Times(1);
    auto loader = register_test_module().with_files(files).build();
    auto settings = loader->load_user_settings();
}

TEST(SettingsLoader, FileSaved)
{
    auto files = std::make_shared<MockFiles>();
    EXPECT_CALL(*files, appdata_directory).Times(1).WillRepeatedly(Return("appdata"));
    EXPECT_CALL(*files, save_file("appdata\\trview\\settings.txt", A<const std::string&>())).Times(1);
    auto loader = register_test_module().with_files(files).build();

    UserSettings settings;
    loader->save_user_settings(settings);
}
