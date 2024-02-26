#pragma once

namespace trview
{
    template <typename T>
    void test(
        ImGuiTestEngine* engine,
        const char* category,
        const char* name,
        ImFuncPtr(ImGuiTestGuiFunc) gui_func,
        ImFuncPtr(ImGuiTestTestFunc) test_func)
    {
        auto t = IM_REGISTER_TEST(engine, category, name);
        t->SetVarsDataType<T>();
        t->GuiFunc = gui_func;
        t->TestFunc = test_func;
    }
}
