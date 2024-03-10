#include "pch.h"
#include "FiltersTests.h"
#include <trview.app/Filters/Filters.h>

using namespace trview;

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

void register_filters_tests(ImGuiTestEngine* engine)
{
    test<Filters<Object>>(engine, "Filters", "Add Filters Using UI",
        [](ImGuiTestContext* ctx) { if (ImGui::Begin("Filters Host")) { ctx->GetVars<Filters<Object>>().render(); ImGui::End(); } },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<Filters<Object>>();
            window.add_getter<float>("value", [](auto&& o) { return o.number; });

            ctx->ItemClick("/**/Filters##FiltersButton");
            ctx->SetRef(ctx->ItemInfo("/**/+")->Window);
            ctx->ItemClick("+");

            ctx->ComboClick("##filter-key-0/value");
            ctx->ComboClick("##filter-compare-op-0/less than");
            ctx->ItemInputValue("##filter-value-0", "10");

            ctx->ItemClick("+");
            ctx->ComboClick("##filter-op-0/Or");
            ctx->ComboClick("##filter-key-1/value");
            ctx->ComboClick("##filter-compare-op-1/greater than");
            ctx->ItemInputValue("##filter-value-1", "15");

            IM_CHECK_EQ(window.match(Object().with_number(12)), false);
            IM_CHECK_EQ(window.match(Object().with_number(9)), true);
            IM_CHECK_EQ(window.match(Object().with_number(16)), true);
        });

    test<Filters<Object>>(engine, "Filters", "Button Toggles Visibility",
        [](ImGuiTestContext* ctx) { if (ImGui::Begin("Filters Host")) { ctx->GetVars<Filters<Object>>().render(); ImGui::End(); } },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<Filters<Object>>();
            IM_CHECK_EQ(ctx->ItemExists("/**/+"), false);
            ctx->ItemClick("/**/Filters##FiltersButton");
            IM_CHECK_EQ(ctx->ItemExists("/**/+"), true);
        });

    test<Filters<Object>>(engine, "Filters", "Checkbox Disables Filters",
        [](ImGuiTestContext* ctx) { if (ImGui::Begin("Filters Host")) { ctx->GetVars<Filters<Object>>().render(); ImGui::End(); } },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<Filters<Object>>();
            window.add_getter<float>("value", [](auto&& o) { return o.number; });
            Filters<Object>::Filter is_float = make_filter().key("value").compare_op(CompareOp::Equal).value("12");
            window.set_filters({ is_float });

            IM_CHECK_EQ(window.match(Object().with_number(13)), false);

            ctx->ItemUncheck("/**/##filter_enabled");

            IM_CHECK_EQ(window.match(Object().with_number(13)), true);
        });

    test<Filters<Object>>(engine, "Filters", "Checkbox Shows Tooltip",
        [](ImGuiTestContext* ctx) { if (ImGui::Begin("Filters Host")) { ctx->GetVars<Filters<Object>>().render(); ImGui::End(); } },
        [](ImGuiTestContext* ctx)
        {
            auto& window = ctx->GetVars<Filters<Object>>();
            ctx->MouseMoveToVoid();
            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), false);
            ctx->MouseMove("/**/##filter_enabled");
            ctx->Yield(10);
            IM_CHECK_EQ(ctx->ItemExists("/**/##Tooltip_00"), true);
        });
}

