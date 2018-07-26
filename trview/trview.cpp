// trview.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "trview.h"
#include "resource.h"

#include "Viewer.h"
#include <memory>
#include <commdlg.h>
#include <shellapi.h>
#include <Shlwapi.h>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<trview::Viewer> viewer;
HWND window;

std::wstring get_exe_directory()
{
    std::vector<wchar_t> exe_directory(MAX_PATH);
    GetModuleFileName(nullptr, &exe_directory[0], static_cast<uint32_t>(exe_directory.size()));
    PathRemoveFileSpec(&exe_directory[0]);
    return std::wstring(exe_directory.begin(), exe_directory.end());
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TRVIEW, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);


    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRVIEW));

    // Set the current directory to the directory that the exe is running from
    // so that the shaders can be found.
    SetCurrentDirectory(get_exe_directory().c_str());

    viewer = std::make_unique<trview::Viewer>(window);

    // Open the level passed in on the command line, if there is one.
    int number_of_arguments = 0;
    const LPWSTR* const arguments = CommandLineToArgvW(GetCommandLine(), &number_of_arguments);
    if (number_of_arguments > 1)
    {
        viewer->open(arguments[1]);
    }

    MSG msg;
    memset(&msg, 0, sizeof(msg));

    while (msg.message != WM_QUIT)
    {
        while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                break;
            }

            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        viewer->render();
    }

    return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRVIEW));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TRVIEW);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   window = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!window)
   {
      return FALSE;
   }

   ShowWindow(window, nCmdShow);
   UpdateWindow(window);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);

            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_FILE_OPEN:
            case ID_ACCEL_FILE_OPEN:
            {
                wchar_t cd[MAX_PATH];
                GetCurrentDirectoryW(MAX_PATH, cd);

                OPENFILENAME ofn;
                memset(&ofn, 0, sizeof(ofn));

                wchar_t path[MAX_PATH];
                memset(&path, 0, sizeof(path));

                ofn.lStructSize = sizeof(ofn);
                ofn.lpstrFile = path;
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrTitle = L"Open level";
                ofn.lpstrFilter = L"All Tomb Raider Files\0*.tr*;*.phd\0";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                if (GetOpenFileName(&ofn))
                {
                    SetCurrentDirectory(cd);
                    viewer->open(ofn.lpstrFile);
                }
                break;
            } 
            case ID_HELP_GITHUB:
            {
                ShellExecute(0, 0, L"https://github.com/chreden/trview", 0, 0, SW_SHOW);
                break;
            }
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
