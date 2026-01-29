#include <trview.app/Filters/Filters.h>

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
        std::optional<float> option;

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

        Object with_option(float value)
        {
            option = value;
            return *this;
        }

    };

    auto make_filter()
    {
        struct FilterBuilder
        {
            Filters::Filter filter;

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

            FilterBuilder& invert(bool value)
            {
                filter.invert = value;
                return *this;
            }

            operator Filters::Filter() const
            {
                return filter;
            }
        };

        return FilterBuilder{};
    };
}

using Names = Filters::Names;

TEST(Filters, IsFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter is_float = make_filter().key("value").compare_op(CompareOp::Equal).value("12");
    filters.set_filters({ is_float });

    ASSERT_TRUE(filters.match(Object().with_number(12)));
    ASSERT_FALSE(filters.match(Object().with_number(0)));
}

TEST(Filters, IsNotFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter is_not_float = make_filter().key("value").compare_op(CompareOp::NotEqual).value("12");
    filters.set_filters({ is_not_float });

    ASSERT_TRUE(filters.match(Object().with_number(0)));
    ASSERT_FALSE(filters.match(Object().with_number(12)));
}

TEST(Filters, GreaterThanFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter gt_float = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("15");
    filters.set_filters({ gt_float });

    ASSERT_TRUE(filters.match(Object().with_number(16)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, GreaterThanEqualFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter gte_float = make_filter().key("value").compare_op(CompareOp::GreaterThanOrEqual).value("15");
    filters.set_filters({ gte_float });

    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(14)));
}

TEST(Filters, LessThanFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter lt_float = make_filter().key("value").compare_op(CompareOp::LessThan).value("15");
    filters.set_filters({ lt_float });

    ASSERT_TRUE(filters.match(Object().with_number(14)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, LessThanEqualFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter lte_float = make_filter().key("value").compare_op(CompareOp::LessThanOrEqual).value("15");
    filters.set_filters({ lte_float });

    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(16)));
}

TEST(Filters, BetweenFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter b_float = make_filter().key("value").compare_op(CompareOp::Between).value("10").value2("15");
    filters.set_filters({ b_float });

    ASSERT_TRUE(filters.match(Object().with_number(11)));
    ASSERT_TRUE(filters.match(Object().with_number(14)));
    ASSERT_FALSE(filters.match(Object().with_number(10)));
    ASSERT_FALSE(filters.match(Object().with_number(15)));
}

TEST(Filters, BetweenInclusiveFloat)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter bi_float = make_filter().key("value").compare_op(CompareOp::BetweenInclusive).value("10").value2("15");
    filters.set_filters({ bi_float });

    ASSERT_TRUE(filters.match(Object().with_number(10)));
    ASSERT_TRUE(filters.match(Object().with_number(15)));
    ASSERT_FALSE(filters.match(Object().with_number(9)));
    ASSERT_FALSE(filters.match(Object().with_number(16)));
}

TEST(Filters, PresentFloat)
{
    Filters filters;
    filters.add_multi_getter<float>("value", [](auto&& o) { return o.numbers; });

    Filters::Filter present_float = make_filter().key("value").compare_op(CompareOp::Exists);
    filters.set_filters({ present_float });

    ASSERT_TRUE(filters.match(Object().with_numbers({ 1, 3, 12, 15 })));
    ASSERT_FALSE(filters.match(Object().with_numbers({ })));
}

TEST(Filters, IsString)
{
    Filters filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters::Filter is_string = make_filter().key("value").compare_op(CompareOp::Equal).value("first");
    filters.set_filters({ is_string });

    ASSERT_TRUE(filters.match(Object().with_text("first")));
    ASSERT_FALSE(filters.match(Object().with_text("second")));
}

TEST(Filters, IsNotString)
{
    Filters filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters::Filter is_not_string = make_filter().key("value").compare_op(CompareOp::NotEqual).value("first");
    filters.set_filters({ is_not_string });

    ASSERT_TRUE(filters.match(Object().with_text("second")));
    ASSERT_FALSE(filters.match(Object().with_text("first")));
}

TEST(Filters, PresentString)
{
    Filters filters;
    filters.add_multi_getter<std::string>("value", [](auto&& o) { return o.texts; });

    Filters::Filter present_string = make_filter().key("value").compare_op(CompareOp::Exists);
    filters.set_filters({ present_string });

    ASSERT_TRUE(filters.match(Object().with_texts({ "test", "string" })));
    ASSERT_FALSE(filters.match(Object().with_texts({ })));
}

TEST(Filters, StartsWithString)
{
    Filters filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters::Filter starts_with_string = make_filter().key("value").compare_op(CompareOp::StartsWith).value("fir");
    filters.set_filters({ starts_with_string });

    ASSERT_TRUE(filters.match(Object().with_text("first")));
    ASSERT_FALSE(filters.match(Object().with_text("second")));
    ASSERT_FALSE(filters.match(Object().with_text("FIRST")));
}

TEST(Filters, EndsWithString)
{
    Filters filters;
    filters.add_getter<std::string>("value", [](auto&& o) { return o.text; });

    Filters::Filter ends_with_string = make_filter().key("value").compare_op(CompareOp::EndsWith).value("st");
    filters.set_filters({ ends_with_string });

    ASSERT_TRUE(filters.match(Object().with_text("first")));
    ASSERT_FALSE(filters.match(Object().with_text("second")));
    ASSERT_FALSE(filters.match(Object().with_text("FIRST")));
}

TEST(Filters, Or)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter value_gt = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("10").op(Op::Or);
    Filters::Filter value_lt = make_filter().key("value").compare_op(CompareOp::LessThan).value("5");
    filters.set_filters({ value_gt, value_lt });

    ASSERT_FALSE(filters.match(Object().with_number(7)));
    ASSERT_TRUE(filters.match(Object().with_number(4)));
    ASSERT_TRUE(filters.match(Object().with_number(11)));
}

TEST(Filters, And)
{
    Filters filters;
    filters.add_getter<float>("value", [](auto&& o) { return o.number; });

    Filters::Filter value_gt = make_filter().key("value").compare_op(CompareOp::GreaterThan).value("5").op(Op::And);
    Filters::Filter value_lt = make_filter().key("value").compare_op(CompareOp::LessThan).value("10");
    filters.set_filters({ value_gt, value_lt });

    ASSERT_TRUE(filters.match(Object().with_number(7)));
    ASSERT_FALSE(filters.match(Object().with_number(5)));
    ASSERT_FALSE(filters.match(Object().with_number(10)));
}

TEST(Filters, NotPresentFloat)
{
    Filters filters;
    filters.add_multi_getter<float>("value", [](auto&& o) { return o.numbers; });

    Filters::Filter present_float = make_filter().key("value").compare_op(CompareOp::NotExists);
    filters.set_filters({ present_float });

    ASSERT_FALSE(filters.match(Object().with_numbers({ 1, 3, 12, 15 })));
    ASSERT_TRUE(filters.match(Object().with_numbers({ })));
}

TEST(Filters, NotPresentString)
{
    Filters filters;
    filters.add_multi_getter<std::string>("value", [](auto&& o) { return o.texts; });

    Filters::Filter present_string = make_filter().key("value").compare_op(CompareOp::NotExists);
    filters.set_filters({ present_string });

    ASSERT_FALSE(filters.match(Object().with_texts({ "test", "string" })));
    ASSERT_TRUE(filters.match(Object().with_texts({ })));
}

TEST(Filters, PresentSingleWithPredicate)
{
    Filters filters;
    filters.add_getter<float>("option", [](auto&& o) { return o.option.value(); }, [](auto&& o) { return o.option.has_value(); });

    Filters::Filter present = make_filter().key("option").compare_op(CompareOp::Exists);
    filters.set_filters({ present });

    ASSERT_TRUE(filters.match(Object().with_option(1.0f)));
    ASSERT_FALSE(filters.match(Object()));
}

TEST(Filters, NotPresentSingleWithPredicate)
{
    Filters filters;
    filters.add_getter<float>("option", [](auto&& o) { return o.option.value(); }, [](auto&& o) { return o.option.has_value(); });

    Filters::Filter present = make_filter().key("option").compare_op(CompareOp::NotExists);
    filters.set_filters({ present });

    ASSERT_TRUE(filters.match(Object()));
    ASSERT_FALSE(filters.match(Object().with_option(1.0f)));
}

TEST(Filters, IsNotMultiValue)
{
    Filters filters;
    filters.add_multi_getter<std::string>("value", [](auto&& o) { return o.texts; });

    Filters::Filter is_not_string = make_filter().key("value").compare_op(CompareOp::NotEqual).value("first");
    filters.set_filters({ is_not_string });

    ASSERT_TRUE(filters.match(Object().with_texts({ "second", "third", "fourth" })));
    ASSERT_FALSE(filters.match(Object().with_texts({ "second", "first", "third" })));
}

TEST(Filters, NestedFilter)
{
    Filters filters;
    filters.add_getter<float>("number", [](auto&& o) { return o.number; });
    filters.add_getter<std::string>("text", [](auto&& o) { return o.text; });

    // Filter: (number == 1 || number == 5) && text == "test"

    // Branch 1
    Filters::Filter number_1 = make_filter().key("number").compare_op(CompareOp::Equal).value("1").op(Op::Or);
    Filters::Filter number_5 = make_filter().key("number").compare_op(CompareOp::Equal).value("5");
    Filters::Filter numbers = make_filter().op(Op::And);
    numbers.children = { number_1, number_5 };

    // Branch 2
    Filters::Filter text_test = make_filter().key("text").compare_op(CompareOp::Equal).value("test");

    Filters::Filter top_filter = make_filter();
    top_filter.children = { numbers, text_test };

    filters.add_filter(top_filter);

    ASSERT_TRUE(filters.match(Object().with_number(1).with_text("test")));
    ASSERT_TRUE(filters.match(Object().with_number(5).with_text("test")));
    ASSERT_FALSE(filters.match(Object().with_number(2).with_text("test2")));
    ASSERT_FALSE(filters.match(Object().with_number(0).with_text("test")));
    ASSERT_FALSE(filters.match(Object().with_number(1).with_text("test2")));
    ASSERT_FALSE(filters.match(Object().with_number(5).with_text("test2")));
}

TEST(Filters, DoubleNested)
{
    Filters filters;
    filters.add_getter<float>("number", [](auto&& o) { return o.number; });
    filters.add_getter<std::string>("text", [](auto&& o) { return o.text; });
    filters.add_getter<float>("option", [](auto&& o) { return o.option.value(); }, [](auto&& o) { return o.option.has_value(); });

    // Filter: ((number == 1 || number == 5) && text == "test") || option

    // Branch 1
    Filters::Filter number_1 = make_filter().key("number").compare_op(CompareOp::Equal).value("1").op(Op::Or);
    Filters::Filter number_5 = make_filter().key("number").compare_op(CompareOp::Equal).value("5");
    Filters::Filter numbers = make_filter().op(Op::And);
    numbers.children = { number_1, number_5 };

    // Branch 2
    Filters::Filter text_test = make_filter().key("text").compare_op(CompareOp::Equal).value("test");

    Filters::Filter left = make_filter().op(Op::Or);
    left.children = { numbers, text_test };

    // Branch 3
    Filters::Filter option = make_filter().key("option").compare_op(CompareOp::Exists);

    Filters::Filter top_filter = make_filter();
    top_filter.children = { left, option };

    filters.add_filter(top_filter);

    ASSERT_TRUE(filters.match(Object().with_number(1).with_text("test")));
    ASSERT_TRUE(filters.match(Object().with_number(5).with_text("test")));
    ASSERT_FALSE(filters.match(Object().with_number(2).with_text("test2")));
    ASSERT_FALSE(filters.match(Object().with_number(0).with_text("test")));
    ASSERT_FALSE(filters.match(Object().with_number(1).with_text("test2")));
    ASSERT_FALSE(filters.match(Object().with_number(5).with_text("test2")));
    ASSERT_TRUE(filters.match(Object().with_number(5).with_text("test2").with_option(0)));
    ASSERT_TRUE(filters.match(Object().with_number(5).with_text("test").with_option(0)));
}

TEST(Filters, UnaryNot)
{
    Filters filters;
    filters.add_getter<std::string>("text", [](auto&& o) { return o.text; });

    // Filter: !(text starts with s)
    Filters::Filter text = make_filter().key("text").compare_op(CompareOp::StartsWith).value("s").invert(true);

    filters.add_filter(text);

    ASSERT_TRUE(filters.match(Object().with_text("test")));
    ASSERT_FALSE(filters.match(Object().with_text("six")));
}
