#include "pch.h"
#include "TestHelpers.h"

namespace trview
{
    std::string ItemText(ImGuiTestContext* ctx, ImGuiTestRef ref)
    {
        for (const auto& t : ctx->Engine->Texts)
        {
            if (t.ID == ref.ID)
            {
                return t.text;
            }
        }
        return std::string{};
    }

    std::string RenderedText(ImGuiTestContext* ctx, ImGuiTestRef ref)
    {
        for (const auto& t : ctx->Engine->RenderedTexts)
        {
            if (t.ID == ref.ID)
            {
                return t.text;
            }
        }
        return std::string{};
    }
}