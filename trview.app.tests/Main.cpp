#include "gtest/gtest.h"

void ImGuiTrviewTestEngineHook_ItemText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
}

void ImGuiTrviewTestEngineHook_RenderedText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
}

int wmain(int argc, wchar_t** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}