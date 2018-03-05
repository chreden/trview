// trview.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "trview.h"
#include "resource.h"

#include "Viewer.h"
#include <memory>
#include <commdlg.h>

#define MAX_LOADSTRING 100

namespace
{
    const int ID_RECENT_FILE_BASE = 5000;
}

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, HWND& window);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

std::unique_ptr<trview::Viewer> viewer;
HWND window;
std::vector<std::wstring> recent_files;

void update_menu(std::list<std::wstring> files)
{
    // Copy recent files locally
    recent_files.assign(files.begin(), files.end());

    // Set up the recently used files menu.
    HMENU menu = GetMenu(window);
    HMENU popup = CreatePopupMenu();

    for(int i = 0; i < recent_files.size(); ++i)
    {
        AppendMenu(popup, MF_STRING, ID_RECENT_FILE_BASE + i, recent_files[i].c_str());
    }

    MENUITEMINFO info;
    memset(&info, 0, sizeof(info));
    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;
    info.hSubMenu = popup;
    SetMenuItemInfo(menu, ID_FILE_OPENRECENT, FALSE, &info);
    SetMenu(window, menu);
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
    if (!InitInstance (hInstance, nCmdShow, window))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRVIEW));

    viewer = std::make_unique<trview::Viewer>(window);
    viewer->on_recent_files_changed += update_menu;

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

            switch (msg.message)
            {
                case WM_KEYDOWN:
                {
                    viewer->on_key_down(static_cast<uint16_t>(msg.wParam));
                    break;
                }
                case WM_CHAR:
                {
                    viewer->on_char(static_cast<uint16_t>(msg.wParam));
                    break;
                }
                case WM_KEYUP:
                {
                    viewer->on_key_up(static_cast<uint16_t>(msg.wParam));
                    break;
                }
                case WM_INPUT:
                {
                    HRAWINPUT input_handle = reinterpret_cast<HRAWINPUT>(msg.lParam);

                    uint32_t size = 0;
                    GetRawInputData(input_handle, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

                    std::vector<uint8_t> data_buffer(size);
                    GetRawInputData(input_handle, RID_INPUT, &data_buffer[0], &size, sizeof(RAWINPUTHEADER));

                    RAWINPUT& data = *reinterpret_cast<RAWINPUT*>(&data_buffer[0]);
                    
                    viewer->on_input(data);
                    break;
                }
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, HWND& window)
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

            // Handle recent files.
            if (wmId >= ID_RECENT_FILE_BASE && wmId <= ID_RECENT_FILE_BASE + recent_files.size())
            {
                int index = wmId - ID_RECENT_FILE_BASE;
                if (index >= 0 && index < recent_files.size())
                {
                    viewer->open(recent_files[index]);
                }
                break;
            }

            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_FILE_OPEN:
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
                ofn.lpstrFilter = L"TR2 Files\0*.tr2\0";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                ofn.nFilterIndex = -1;

                if (GetOpenFileName(&ofn))
                {
                    SetCurrentDirectory(cd);
                    viewer->open(ofn.lpstrFile);
                }
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
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
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
