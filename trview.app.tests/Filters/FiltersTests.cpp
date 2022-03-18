#include <trview.app/Filters/Filters.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

namespace
{
    struct Object
    {
        float number = 0;
        std::vector<float> numbers;
        std::string text;
        std::vector<std::string> texts;

        Object with_number(float value)
        {
            number = value;
            return *this;
        }

        Object with_numbers(const std::vector<float>& value)
        {
            numbers = value;
            return *this;
        }

        Object with_text(const std::string& value)
        {
            text = value;
            return *this;
        }

        Object with_texts(const std::vector<std::string>& value)
        {
            texts = value;
            return *this;
        }
    };

    auto make_filter()
    {
        struct FilterBuilder
        {
            Filters<Object>::Filter filter;

            FilterBuilder& compare_op(CompareOp compare)
            {
                filter.compare = compare;
                return *this;
            }

            FilterBuilder& op(Op op)
            {
                filter.op = op;
                return *this;
            }

            FilterBuilder& key(const std::string& key)
            {
                filter.key = key;
                return *this;
            }

            FilterBuilder& value(const std::string& value)
            {
                filter.value = value;
                return *this;
            }

            FilterBuilder& value2(const std::string& value2)
            {
                filter.value2 = value2;
                return *this;
            }

            operator Filters<Object>::Filter() const
            {
                return filter;
            }
        };

        return FilterBuilder{};
    };
}

using Names = Filters<Object>::Names;

TEST(Filters, ButtonTogglesVisibility)
{
    Filters<Object> filters;
    TestImgui imgui([&]() { filters.render(); });
    ASSERT_EQ(imgui.find_window(imgui.popup_id(Names::Popup).name()), nullptr);
    imgui.click_element(imgui.id("Debug##Default").id(Names::FiltersButton));
    ASSERT_NE(imgui.find_window(imgui.popup_id(Names::Popup).name()), nullptr);
}

TEST(Filters, CheckboxDisablesFilters)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });
    Filters<Object>::Filter is_float = make_filter().key("value").compare_op(CompareOp::Equal).value("12");
    filters.set_filters({ is_float });

    ASSERT_FALSE(filters.match(Object().with_number(13)));

    TestImgui imgui([&]() { filters.render(); });
    imgui.click_element(imgui.id("Debug##Default").id(Names::Enable));

    ASSERT_TRUE(filters.match(Object().with_number(13)));
}

TEST(Filters, AddFilterUsingUI)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });
    
    TestImgui imgui([&]() { filters.render(); });
    ASSERT_EQ(imgui.find_window(imgui.popup_id(Names::Popup).name()), nullptr);
    imgui.click_element(imgui.id("Debug##Default").id(Names::FiltersButton));
    ASSERT_NE(imgui.find_window(imgui.popup_id(Names::Popup).name()), nullptr);

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::AddFilter));
    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterKey + "0"));
    imgui.click_element(imgui.id("##Combo_01").id("value"));

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterCompareOp + "0"));
    imgui.click_element(imgui.id("##Combo_01").id(compare_op_to_string(CompareOp::LessThan)));

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterValue + "0"));
    imgui.enter_text("10");

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::AddFilter));
    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterOp + "0"));
    imgui.click_element(imgui.id("##Combo_01").id(op_to_string(Op::Or)));

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterKey + "1"));
    imgui.click_element(imgui.id("##Combo_01").id("value"));

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterCompareOp + "1"));
    imgui.click_element(imgui.id("##Combo_01").id(compare_op_to_string(CompareOp::GreaterThan)));

    imgui.click_element(imgui.popup_id(Names::Popup).id(Names::FilterValue + "1"));
    imgui.enter_text("15");

    ASSERT_FALSE(filters.match(Object().with_number(12)));
    ASSERT_TRUE(filters.match(Object().with_number(9)));
    ASSERT_TRUE(filters.match(Object().with_number(16)));
}

TEST(Filters, IsFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter is_float = make_filter().key("value").compare_op(CompareOp::Equal).value("12");
    filters.set_filters({ is_float });

    ASSERT_TRUE(filters.match(Object().with_number(12)));
    ASSERT_FALSE(filters.match(Object().with_number(0)));
}

TEST(Filters, IsNotFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter is_not_float = make_filter().key("value").compare_op(CompareOp::NotEqual).value("12");
    filters.set_filters({ is_not_float });

    ASSERT_TRUE(filters.match(Object().with_number(0)));
    ASSERT_FALSE(filters.match(Object().with_number(12)));
}

TEST(Filters, GreaterThanFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter gt_float = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("15");
    filters.set_filters({ gt_float });

    ASSERT_TRUE(filters.match(Object().with_number(16)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, GreaterThanEqualFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter gte_float = make_filter().key("value").compare_op(CompareOp::GreaterThanOrEqual).value("15");
    filters.set_filters({ gte_float });

    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(14)));
}

TEST(Filters, LessThanFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter lt_float = make_filter().key("value").compare_op(CompareOp::LessThan).value("15");
    filters.set_filters({ lt_float });

    ASSERT_TRUE(filters.match(Object().with_number(14)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, LessThanEqualFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter lte_float = make_filter().key("value").compare_op(CompareOp::LessThanOrEqual).value("15");
    filters.set_filters({ lte_float });

    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(16)));
}

TEST(Filters, BetweenFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter b_float = make_filter().key("value").compare_op(CompareOp::Between).value("10").value2("15");
    filters.set_filters({ b_float });

    ASSERT_TRUE(filters.match(Object().with_number(11)));
    ASSERT_TRUE(filters.match(Object().with_number(14)));
    ASSERT_FALSE(filters.match(Object().with_number(10)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, BetweenInclusiveFloat)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter bi_float = make_filter().key("value").compare_op(CompareOp::BetweenInclusive).value("10").value2("15");
    filters.set_filters({ bi_float });

    ASSERT_TRUE(filters.match(Object().with_number(10)));
    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(9)));
    ASSERT_FALSE(filters.match(Object().with_number(16)));
}

TEST(Filters, PresentFloat)
{
    Filters<Object> filters;
    filters.add_multi_getter<float>("value", [](auto&& o) { return o.numbers; });

    Filters<Object>::Filter present_float = make_filter().key("value").compare_op(CompareOp::Exists);
    filters.set_filters({ present_float });

    ASSERT_TRUE(filters.match(Object().with_numbers({ 1, 3, 12, 15 })));
    ASSERT_FALSE(filters.match(Object().with_numbers({ })));
}

TEST(Filters, IsString)
{
    Filters<Object> filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters<Object>::Filter is_string = make_filter().key("value").compare_op(CompareOp::Equal).value("first");
    filters.set_filters({ is_string });

    ASSERT_TRUE(filters.match(Object().with_text("first")));
    ASSERT_FALSE(filters.match(Object().with_text("second")));
}

TEST(Filters, IsNotString)
{
    Filters<Object> filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters<Object>::Filter is_not_string = make_filter().key("value").compare_op(CompareOp::NotEqual).value("first");
    filters.set_filters({ is_not_string });

    ASSERT_TRUE(filters.match(Object().with_text("second")));
    ASSERT_FALSE(filters.match(Object().with_text("first")));
}

TEST(Filters, PresentString)
{
    Filters<Object> filters;
    filters.add_multi_getter<std::string>("value", [](auto&& o) { return o.texts; });

    Filters<Object>::Filter present_string = make_filter().key("value").compare_op(CompareOp::Exists);
    filters.set_filters({ present_string });

    ASSERT_TRUE(filters.match(Object().with_texts({ "test", "string" })));
    ASSERT_FALSE(filters.match(Object().with_texts({ })));
}

TEST(Filters, Or)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter value_gt = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("10").op(Op::Or);
    Filters<Object>::Filter value_lt = make_filter().key("value").compare_op(CompareOp::LessThan).value("5");
    filters.set_filters({ value_gt, value_lt });

    ASSERT_FALSE(filters.match(Object().with_number(7)));
    ASSERT_TRUE(filters.match(Object().with_number(4)));
    ASSERT_TRUE(filters.match(Object().with_number(11)));
}

TEST(Filters, And)
{
    Filters<Object> filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters<Object>::Filter value_gt = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("5").op(Op::And);
    Filters<Object>::Filter value_lt = make_filter().key("value").compare_op(CompareOp::LessThan).value("10");
    filters.set_filters({ value_gt, value_lt });

    ASSERT_TRUE(filters.match(Object().with_number(7)));
    ASSERT_FALSE(filters.match(Object().with_number(5)));
    ASSERT_FALSE(filters.match(Object().with_number(10)));
}
