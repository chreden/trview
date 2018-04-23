// trview.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "trview.h"
#include "resource.h"

#include "Viewer.h"
#include <memory>
#include <commdlg.h>
#include <shellapi.h>

#include "DirectoryListing.h"

#define MAX_LOADSTRING 100

namespace
{
    const int ID_RECENT_FILE_BASE = 5000;
    const int ID_SWITCHFILE_BASE = 10000;
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

// helper object to list all of the files in a directory 
trview::DirectoryListing dir_lister;
// list of files in directory, updated by dir_lister
std::vector<trview::File> file_switcher_list;

HMENU directory_listing_menu;

bool resizing = false;

void create_directory_listing_menu()
{
    HMENU menu = GetMenu(window);
    directory_listing_menu = CreatePopupMenu(); 

    MENUITEMINFO info;
    memset(&info, 0, sizeof(info));

    info.cbSize = sizeof(info);
    info.fMask = MIIM_SUBMENU;
    info.hSubMenu = directory_listing_menu;

    // Set up the popup menu and grey it out initially - only when it's populated do we enable it
    SetMenuItemInfo(menu, ID_FILE_SWITCHLEVEL, FALSE, &info);
    EnableMenuItem(menu, ID_FILE_SWITCHLEVEL, MF_GRAYED);

    SetMenu(window, menu);

    DrawMenuBar(window);
}

void reset_menu(HMENU menu)
{
    int count = GetMenuItemCount(menu);

    while ((count = GetMenuItemCount(menu)) > 0)
        RemoveMenu(menu, 0, MF_BYPOSITION);

    DrawMenuBar(window);
}

void populate_directory_listing_menu(const std::wstring& filepath)
{
    const std::size_t pos = filepath.find_last_of(L"\\/");
    const std::wstring folder = filepath.substr(0, pos);

    dir_lister.SetDirectory(folder);
    file_switcher_list = dir_lister.GetFiles();

    // Enable menu when populating in case it's not enabled
    EnableMenuItem(GetMenu(window), ID_FILE_SWITCHLEVEL, MF_ENABLED);

    // Clear all items from menu and repopulate
    reset_menu(directory_listing_menu);
    for (int i = 0; i < file_switcher_list.size(); ++i)
        AppendMenuW(directory_listing_menu, MF_STRING, ID_SWITCHFILE_BASE + i, file_switcher_list.at(i).friendly_name.c_str());

    DrawMenuBar(window);
}

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

std::wstring get_exe_directory()
{
    std::vector<wchar_t> exe_directory(MAX_PATH);
    GetModuleFileName(nullptr, &exe_directory[0], exe_directory.size());
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
    if (!InitInstance (hInstance, nCmdShow, window))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TRVIEW));

    // Set the current directory to the directory that the exe is running from
    // so that the shaders can be found.
    SetCurrentDirectory(get_exe_directory().c_str());

    viewer = std::make_unique<trview::Viewer>(window);
    // Register for future updates to the recent files list.
    viewer->on_recent_files_changed += update_menu;
    // Make sure the menu has the values loaded from the settings file.
    update_menu(viewer->settings().recent_files);
    // Create current directory files menu entry.
    create_directory_listing_menu();
	// Makes this window accept dropped files.
	DragAcceptFiles(window, TRUE);

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
				case WM_DROPFILES:
				{
					wchar_t filename[MAX_PATH];
					memset(&filename, 0, sizeof(filename));
					DragQueryFile((HDROP)msg.wParam, 0, filename, MAX_PATH);
					viewer->open(filename);
					populate_directory_listing_menu(filename);
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
                    const auto file = recent_files[index];
                    viewer->open(file);
                    populate_directory_listing_menu(file);
                }
                break;
            }
            else if (wmId >= ID_SWITCHFILE_BASE 
                && wmId <= (ID_SWITCHFILE_BASE + GetMenuItemCount(directory_listing_menu)))
            {
                const trview::File& f = file_switcher_list.at(wmId - ID_SWITCHFILE_BASE);
                viewer->open(f.path);
                
                break; // We don't need to refresh the file switcher as it should be the same directory
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
                ofn.lpstrFilter = L"All Tomb Raider Files\0*.tr*;*.phd\0";
                ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                ofn.nFilterIndex = -1;

                if (GetOpenFileName(&ofn))
                {
                    SetCurrentDirectory(cd);
                    viewer->open(ofn.lpstrFile);

                    populate_directory_listing_menu(ofn.lpstrFile);
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
    case WM_ENTERSIZEMOVE:
    {
        resizing = true;
        break;
    }
    case WM_SIZE:
    {
        if (viewer && !resizing && (wParam == SIZE_MAXIMIZED || wParam == SIZE_RESTORED))
        {
            viewer->resize();
        }
        break;
    }
    case WM_EXITSIZEMOVE:
    {
        resizing = false;
        if (viewer)
        {
            viewer->resize();
        }
        return 0;
    }
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
