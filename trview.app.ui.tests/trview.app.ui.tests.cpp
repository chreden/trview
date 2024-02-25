// trview.app.ui.tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

#define IMGUI_TEST_ENGINE_ENABLE_COROUTINE_STDTHREAD_IMPL 1

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_app.h>
// #include <imgui_te_engine.h>
#include <imgui_te_context.h>
#include <imgui_te_ui.h>

void ImGuiTrviewTestEngineHook_ItemText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
}

void ImGuiTrviewTestEngineHook_RenderedText(ImGuiContext* ctx, ImGuiID id, const char* buf)
{
}

void register_view_options_tests(ImGuiTestEngine* engine)
{
    // Register tests
    auto t = IM_REGISTER_TEST(engine, "demo_tests", "test1");
    t->GuiFunc = [](ImGuiTestContext* ctx)
    {
        IM_UNUSED(ctx);
        ImGui::Begin("Test Window", NULL, ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("Hello, automation world");
        ImGui::Button("Click Me");
        if (ImGui::TreeNode("Node"))
        {
            static bool b = false;
            ImGui::Checkbox("Checkbox", &b);
            ImGui::TreePop();
        }
        ImGui::End();
    };
    t->TestFunc = [](ImGuiTestContext* ctx)
    {
        ctx->SetRef("Test Window");
        ctx->ItemClick("Click Me");
        ctx->ItemOpen("Node"); // Optional as ItemCheck("Node/Checkbox") can do it automatically
        ctx->ItemCheck("Node/Checkbox");
        ctx->ItemUncheck("Node/Checkbox");
    };
}

int main()
{
    // Setup application backend
    ImGuiApp* app = ImGuiApp_ImplDefault_Create();

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
#ifdef IMGUI_HAS_DOCK
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#endif
#ifdef IMGUI_HAS_VIEWPORT
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
#if IMGUI_VERSION_NUM >= 19004
    io.ConfigDebugIsDebuggerPresent = ImOsIsDebuggerPresent();
#endif

    // Setup application
    app->DpiAware = false;
    app->SrgbFramebuffer = false;
    app->ClearColor = ImVec4(0.120f, 0.120f, 0.120f, 1.000f);
    app->InitCreateWindow(app, "Dear ImGui: Minimal App With Test Engine", ImVec2(1600, 1000));
    app->InitBackends(app);

    // Setup test engine
    ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Cinematic; // Default to slowest mode in this demo
    test_io.ScreenCaptureFunc = ImGuiApp_ScreenCaptureFunc;
    test_io.ScreenCaptureUserData = (void*)app;

    // Optional: save test output in junit-compatible XML format.
    //test_io.ExportResultsFile = "./results.xml";
    // test_io.ExportResultsFormat = ImGuiTestEngineExportFormat_JUnitXml;

    // Start test engine
    ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
    ImGuiTestEngine_InstallDefaultCrashHandler();

    register_view_options_tests(engine);

    // Main loop
    bool aborted = false;
    while (!aborted)
    {
        if (!aborted && !app->NewFrame(app))
            aborted = true;
        if (app->Quit)
            aborted = true;

        if (aborted && ImGuiTestEngine_TryAbortEngine(engine))
            break;

        ImGui::NewFrame();

        // Show windows
        ImGui::ShowDemoWindow();
        ImGuiTestEngine_ShowTestEngineWindows(engine, NULL);

        // Render and swap
        app->Vsync = test_io.IsRequestingMaxAppSpeed ? false : true;
        ImGui::Render();
        app->Render(app);

        // Post-swap handler is REQUIRED in order to support screen capture
        ImGuiTestEngine_PostSwap(engine);
    }

    // Shutdown
    ImGuiTestEngine_Stop(engine);
    app->ShutdownBackends(app);
    app->ShutdownCloseWindow(app);
    ImGui::DestroyContext();

    // IMPORTANT: we need to destroy the Dear ImGui context BEFORE the test engine context, so .ini data may be saved.
    ImGuiTestEngine_DestroyContext(engine);

    app->Destroy(app);



    /*
    ImGuiApp* app = ImGuiApp_ImplDefault_Create();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    app->DpiAware = true;
    app->Vsync = true;
    app->InitCreateWindow(app, "My Application", ImVec2(1600, 1200));
    app->InitBackends(app);

    ImGuiTestEngine* engine = ImGuiTestEngine_CreateContext();
    ImGuiTestEngineIO& test_io = ImGuiTestEngine_GetIO(engine);
    test_io.ConfigVerboseLevel = ImGuiTestVerboseLevel_Info;
    test_io.ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Debug;
    test_io.ConfigRunSpeed = ImGuiTestRunSpeed_Cinematic; // Default to slowest mode in this demo
    test_io.ScreenCaptureFunc = ImGuiApp_ScreenCaptureFunc;
    test_io.ScreenCaptureUserData = (void*)app;
    test_io.ConfigSavedSettings = false;

    ImGuiTestEngine_Start(engine, ImGui::GetCurrentContext());
    ImGuiTestEngine_InstallDefaultCrashHandler();

    auto t = IM_REGISTER_TEST(engine, "demo_tests", "test1");
    t->GuiFunc = [](ImGuiTestContext* ctx)
    {
        IM_UNUSED(ctx);
        ImGui::Begin("Test Window", NULL, ImGuiWindowFlags_NoSavedSettings);
        ImGui::Text("Hello, automation world");
        ImGui::Button("Click Me");
        if (ImGui::TreeNode("Node"))
        {
            static bool b = false;
            ImGui::Checkbox("Checkbox", &b);
            ImGui::TreePop();
        }
        ImGui::End();
    };
    t->TestFunc = [](ImGuiTestContext* ctx)
    {
        ctx->SetRef("Test Window");
        ctx->ItemClick("Click Me");
        ctx->ItemOpen("Node"); // Optional as ItemCheck("Node/Checkbox") can do it automatically
        ctx->ItemCheck("Node/Checkbox");
        ctx->ItemUncheck("Node/Checkbox");
    };

    bool aborted = false;
    while (!aborted)
    {
        if (!aborted && !app->NewFrame(app))
            aborted = true;
        if (app->Quit)
            aborted = true;

        if (aborted && ImGuiTestEngine_TryAbortEngine(engine))
            break;

        ImGui::NewFrame();

        ImGuiTestEngine_ShowTestEngineWindows(engine, NULL);

        // window->render();
        ImGui::Render();
        app->Render(app);
        // Post-swap handler is REQUIRED in order to support screen capture
        ImGuiTestEngine_PostSwap(engine);
    }

    ImGuiTestEngine_Stop(engine);
    app->ShutdownBackends(app);
    app->ShutdownCloseWindow(app);
    ImGui::DestroyContext();

    // IMPORTANT: we need to destroy the Dear ImGui context BEFORE the test engine context, so .ini data may be saved.
    ImGuiTestEngine_DestroyContext(engine);

    app->Destroy(app);
    */
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
