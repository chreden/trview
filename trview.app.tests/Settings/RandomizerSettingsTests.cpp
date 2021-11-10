#include <trview.app/Settings/RandomizerSettings.h>

using namespace trview;

TEST(RandomizerSettings, SettingsOfType)
{
    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean, true };
    settings.settings["test2"] = { "Test 2", RandomizerSettings::Setting::Type::Boolean, false };
    settings.settings["test3"] = { "Test 3", RandomizerSettings::Setting::Type::Boolean, true };
    settings.settings["test4"] = { "Test 4", RandomizerSettings::Setting::Type::String, "Value" };
    settings.settings["test5"] = { "Test 5", RandomizerSettings::Setting::Type::String, "Value 2" };
    settings.settings["test6"] = { "Test 6", RandomizerSettings::Setting::Type::Number, 1.0f };
    ASSERT_EQ(settings.settings_of_type(RandomizerSettings::Setting::Type::Boolean), 3);
    ASSERT_EQ(settings.settings_of_type(RandomizerSettings::Setting::Type::String), 2);
    ASSERT_EQ(settings.settings_of_type(RandomizerSettings::Setting::Type::Number), 1);
}

TEST(RandomizerSettings, Serialize)
{
    RandomizerSettings settings;
    settings.settings["test1"] = { "Test 1", RandomizerSettings::Setting::Type::Boolean, true };
    settings.settings["test2"] = { "Test 2", RandomizerSettings::Setting::Type::String, std::string("One"), { std::string("One"), std::string("Two") } };
    settings.settings["test3"] = { "Test 3", RandomizerSettings::Setting::Type::String, std::string("One") };
    settings.settings["test4"] = { "Test 4", RandomizerSettings::Setting::Type::Number, 1.5f };

    nlohmann::json json;
    to_json(json, settings);

    RandomizerSettings deserialized;
    from_json(json, deserialized);

    ASSERT_NE(settings.settings.find("test1"), settings.settings.end());
    auto test1 = settings.settings["test1"];
    ASSERT_EQ(test1.display, "Test 1");
    ASSERT_EQ(std::get<bool>(test1.default_value), true);
    ASSERT_TRUE(test1.options.empty());
    ASSERT_EQ(test1.type, RandomizerSettings::Setting::Type::Boolean);

    ASSERT_NE(settings.settings.find("test2"), settings.settings.end());
    auto test2 = settings.settings["test2"];
    ASSERT_EQ(test2.display, "Test 2");
    ASSERT_EQ(std::get<std::string>(test2.default_value), std::string("One"));
    ASSERT_EQ(test2.options.size(), 2);
    ASSERT_EQ(std::get<std::string>(test2.options[0]), std::string("One"));
    ASSERT_EQ(std::get<std::string>(test2.options[1]), std::string("Two"));
    ASSERT_EQ(test2.type, RandomizerSettings::Setting::Type::String);

    ASSERT_NE(settings.settings.find("test3"), settings.settings.end());
    auto test3 = settings.settings["test3"];
    ASSERT_EQ(test3.display, "Test 3");
    ASSERT_EQ(std::get<std::string>(test3.default_value), std::string("One"));
    ASSERT_TRUE(test3.options.empty());
    ASSERT_EQ(test3.type, RandomizerSettings::Setting::Type::String);

    ASSERT_NE(settings.settings.find("test4"), settings.settings.end());
    auto test4 = settings.settings["test4"];
    ASSERT_EQ(test4.display, "Test 4");
    ASSERT_EQ(std::get<float>(test4.default_value), 1.5f);
    ASSERT_TRUE(test4.options.empty());
    ASSERT_EQ(test4.type, RandomizerSettings::Setting::Type::Number);
}
