#include <trview.app/Filters/FilterStore.h>
#include <trview.app/Mocks/Filters/IFilterable.h>
#include <trview.common/Mocks/IFiles.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

namespace
{
    TEST(FilterStore, AddAndGet)
    {
        auto files = mock_shared<MockFiles>();
        UserSettings settings;
        FilterStore store(files, settings);

        Filters::Filter filter{ .type_key = "Test" };
        Filters::Filter filter2{ .type_key = "Test" };
        Filters::Filter filter3{ .type_key = "Unrelated" };
        store.add("Test filter 1", filter);
        store.add("Test filter 2", filter2);
        store.add("Test filter 1", filter3);

        auto results = store.filters_for_key("Test");

        ASSERT_EQ(results.size(), 2);

        auto found1 = results.find("Test filter 1");
        ASSERT_NE(found1, results.end());
        ASSERT_EQ(found1->first, "Test filter 1");

        auto found2 = results.find("Test filter 2");
        ASSERT_NE(found2, results.end());
        ASSERT_EQ(found2->first, "Test filter 2");

        auto found3 = results.find("Test filter 1");
        ASSERT_NE(found3, results.end());
        ASSERT_EQ(found3->first, "Test filter 1");
    }

    TEST(FilterStore, LoadTypeKeys)
    {
        const std::vector<IFiles::Directory> directories
        {
            {.path = "Test", .friendly_name = "Test" },
            {.path = "Test 2", .friendly_name = "Test 2" }
        };

        const std::vector<IFiles::File> test_files
        {
            {.path = "Test1_1" },
            {.path = "Test1_2" }
        };

        const std::vector<IFiles::File> test2_files
        {
            {.path = "Test2_1" }
        };

        auto files = mock_shared<MockFiles>();
        ON_CALL(*files, get_directories("dir")).WillByDefault(Return(directories));
        ON_CALL(*files, get_files("Test", "\\*.json")).WillByDefault(Return(test_files));
        ON_CALL(*files, get_files("Test 2", "\\*.json")).WillByDefault(Return(test2_files));
        ON_CALL(*files, load_file("Test1_1")).WillByDefault(Return(std::string("{\"name\":\"Test filter 1\"}") | std::ranges::to<std::vector<uint8_t>>()));
        ON_CALL(*files, load_file("Test1_2")).WillByDefault(Return(std::string("{\"name\":\"Test filter 2\"}") | std::ranges::to<std::vector<uint8_t>>()));
        ON_CALL(*files, load_file("Test2_1")).WillByDefault(Return(std::string("{\"name\":\"Test filter 1\"}") | std::ranges::to<std::vector<uint8_t>>()));

        UserSettings settings{ .filter_directory = "dir" };
        FilterStore store(files, settings);

        store.load();

        auto results = store.filters_for_key("Test");
        auto found1 = results.find("Test filter 1");
        ASSERT_NE(found1, results.end());
        ASSERT_EQ(found1->first, "Test filter 1");

        auto found2 = results.find("Test filter 2");
        ASSERT_NE(found2, results.end());
        ASSERT_EQ(found2->first, "Test filter 2");

        auto results2 = store.filters_for_key("Test 2");
        auto found3 = results2.find("Test filter 1");
        ASSERT_NE(found3, results2.end());
        ASSERT_EQ(found3->first, "Test filter 1");
    }

    TEST(FilterStore, Remove)
    {
        UserSettings settings;
        FilterStore store(mock_shared<MockFiles>(), settings);

        Filters::Filter filter{ .type_key = "Test" };
        store.add("Test filter 1", filter);

        auto results = store.filters_for_key("Test");
        auto found1 = results.find("Test filter 1");
        ASSERT_NE(found1, results.end());
        ASSERT_EQ(found1->first, "Test filter 1");

        store.remove("Test", "Test filter 1");

        results = store.filters_for_key("Test");
        found1 = results.find("Test filter 1");
        ASSERT_EQ(found1, results.end());
    }

    TEST(FilterStore, Save)
    {
        std::string filename;
        std::string data;

        auto files = mock_shared<MockFiles>();
        EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::string&>())).WillOnce(SaveArg<1>(&data));

        UserSettings settings;
        FilterStore store(files, settings);

        Filters::Filter filter{ .type_key = "Test" };
        store.add("Test filter 1", filter);

        store.save();

        std::string result = data;

        ASSERT_EQ(result, "{\"filter\":{\"children\":[],\"compare\":\"Equal\",\"invert\":false,\"key\":\"\",\"op\":\"And\",\"type_key\":\"Test\",\"value\":\"\",\"value2\":\"\"},\"name\":\"Test filter 1\"}");
    }
}