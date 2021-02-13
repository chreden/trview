#include <trview.app/Application.h>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow)
{
    trview::Application application(hInstance, GetCommandLine(), nCmdShow);
    return application.run();
}
