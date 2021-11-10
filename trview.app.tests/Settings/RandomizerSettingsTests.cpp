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
    FAIL();
}

TEST(RandomizerSettings, Deserialize)
{
    FAIL();
}
