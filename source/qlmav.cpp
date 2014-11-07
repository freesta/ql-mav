//  Copyright (C) 2014 freesta
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
//  GNU General Public License for more details.
//  You should have received a copy of the GNU General Public License along
//  with this program; if not, write to the Free Software Foundation, Inc.,
//  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

#include "stdafx.h"
#include "qlmav.h"
#include "Strsafe.h"
#include "Windowsx.h" 
#include "Math.h" 
#include <chrono>
#include <commctrl.h>
#include "conversion.h"

#define MAX_LOADSTRING 100
#define IDC_CONVERT_BUTTON 2000
#define IDC_MCPI_BOX 2001

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND main_window = NULL;
HWND text_box = NULL;
HWND graph_box = NULL;
HWND sens_box = NULL;
HWND accel_box = NULL;
HWND offset_box = NULL;
HWND senscap_box = NULL;
HWND power_box = NULL;
HWND m_cpi_box = NULL;
HWND fps_box = NULL;
HWND x_scale_box = NULL;
HWND y_scale_box = NULL;
HWND controlcontainer = NULL;
HWND caption_box = NULL;
HWND convert_button = NULL;
HWND mcpi_convert_box = NULL;

double graph_max_x = 8;
double graph_max_y = 8;
long graph_width = 400;
long graph_height = 209;

double speed = 0.0;
double cvar_accel = 0;
double cvar_senscap = 0;
double cvar_sens = 4;
double cvar_offset = 0;
double cvar_power = 2;
double cvar_m_cpi = 0;
double max_fps = 125;

double multi = 0.0;
double cur_fps = 0.0;
double cur_mouse_poll_rate = 0.0;

long lastx = 0;
long lasty = 0;

RECT invalidate_rect = { 10, 10, 410, 219 };
RECT invalidate_rect_2 = { 0, 0, 400, 100 };

BITMAP bitmap;
RECT last_pixel;
COLORREF background_color = 0x00f0f0f0; 
COLORREF text_color = 0x00000000;
HBRUSH red_brush = CreateSolidBrush(0x000000FF);
HBRUSH white_brush = CreateSolidBrush(0x00FFFFFF);
HBRUSH black_brush = CreateSolidBrush(0x00000000);
HBRUSH background_brush = CreateSolidBrush(background_color);

HDC          hdc_backbuffer, hdc_backbuffer_2;
HBITMAP      hbm_backbuffer, hbm_backbuffer_2;
HANDLE       h_old, h_old_2;
HFONT standard_font;

auto t_start = std::chrono::high_resolution_clock::now();
auto t_start_short = std::chrono::high_resolution_clock::now();

TCHAR global_temp_string[300];
TCHAR info_string_left[300];
TCHAR info_string_right[300];
TCHAR speed_unit_str_cpms[10] = L"counts/ms";
TCHAR speed_unit_str_cmps[10] = L"cm/s";
TCHAR move_unit_str_cpms[21] = L"deg/(counts*0.022)";
TCHAR move_unit_str_cmps[10] = L"deg/cm";
TCHAR *speed_unit_str = speed_unit_str_cpms;
TCHAR *move_unit_str = move_unit_str_cpms;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	AboutProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ConvertProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	ErrorProc(HWND, UINT, WPARAM, LPARAM);

void ConvertToMcpi(double convert_to_mcpi);
void ConvertFromMcpi();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
  _In_opt_ HINSTANCE hPrevInstance,
  _In_ LPTSTR    lpCmdLine,
  _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  MSG msg;
  HACCEL hAccelTable;
  standard_font = CreateFont(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
    OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_DONTCARE, TEXT("MS Shell Dlg"));

  // Initialize global strings
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_QLMOUSEACCELHELPER, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  INITCOMMONCONTROLSEX InitCtrlEx;

  InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
  InitCtrlEx.dwICC = ICC_LINK_CLASS, ICC_PROGRESS_CLASS, ICC_STANDARD_CLASSES;
  InitCommonControlsEx(&InitCtrlEx);

  // Perform application initialization:
  if (!InitInstance(hInstance, nCmdShow))
  {
    return FALSE;
  }

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QLMOUSEACCELHELPER));

  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0))
  {

    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg) && !IsDialogMessage(main_window, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int)msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  //	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QLMOUSEACCELHELPER
  wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = background_brush;
  wcex.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

  return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  HWND hWnd;

  hInst = hInstance;

  //  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
  //     CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
  hWnd = main_window = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
    CW_USEDEFAULT, CW_USEDEFAULT, 525, 310, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  SendMessage(hWnd, WM_COMMAND, MAKEWPARAM(0, EN_CHANGE), 0);
  UpdateWindow(hWnd);
  //   SetWindowPos(hWnd,HWND_TOPMOST,0,0,444,444,0);
  RAWINPUTDEVICE rid[1];

  rid[0].usUsagePage = 0x01;
  rid[0].usUsage = 0x02;
  rid[0].dwFlags = RIDEV_INPUTSINK;
  rid[0].hwndTarget = hWnd;


  if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE) {
    //registration failed. Call GetLastError for the cause of the error
    MessageBox(NULL, L"Could not register mouse input.", L"Error", MB_OK);
  }


  return TRUE;
}

void CreateToolTipForRect(HWND hwndParent, HINSTANCE ginst, TCHAR* text)
{
  // Create a tooltip.
  HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
    WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    hwndParent, NULL, ginst, NULL);

  SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0,
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
  //SendMessage(hwndTT,TTM_SETMAXTIPWIDTH, 0, 200);  // Multiline-tooltip


  // Set up "tool" information. In this case, the "tool" is the entire parent window.

  TOOLINFO ti = { 0 };
  ti.cbSize = sizeof(TOOLINFO); // This only works if it if -4 is appended when visual styles are disabled.
  ti.uFlags = TTF_SUBCLASS;
  ti.hwnd = hwndParent;
  ti.hinst = ginst;
  ti.lpszText = text;

  GetClientRect(hwndParent, &ti.rect);

  // Associate the tooltip with the "tool" window.
  SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}

LRESULT CALLBACK TextBoxProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  HRESULT hResult;




  switch (message)
  {
  case WM_PAINT:
    hdc = BeginPaint(hWnd, &ps);

    hdc_backbuffer_2 = CreateCompatibleDC(hdc);
    hbm_backbuffer_2 = CreateCompatibleBitmap(hdc, 400, 100);
    h_old_2 = SelectObject(hdc_backbuffer_2, hbm_backbuffer_2);

    FillRect(hdc_backbuffer_2, &invalidate_rect_2, background_brush);

    SetTextColor(hdc_backbuffer_2, 0x00000000);
    SetBkMode(hdc_backbuffer_2, TRANSPARENT);
    SelectObject(hdc_backbuffer_2, standard_font);
    DrawText(hdc_backbuffer_2, info_string_left, wcslen(info_string_left), &invalidate_rect_2, DT_LEFT | DT_EXTERNALLEADING | DT_WORDBREAK);
    DrawText(hdc_backbuffer_2, info_string_right, wcslen(info_string_right), &invalidate_rect_2, DT_RIGHT | DT_EXTERNALLEADING | DT_WORDBREAK);
    BitBlt(hdc, 0, 0, 400, 100, hdc_backbuffer_2, 0, 0, SRCCOPY);


    SelectObject(hdc_backbuffer_2, h_old_2);
    DeleteObject(hbm_backbuffer_2);
    DeleteDC(hdc_backbuffer_2);


    EndPaint(hWnd, &ps);

    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  HRESULT hResult;

  switch (message)
  {

  case WM_CREATE:
  {
    // Todo: 
    // 1. Put this in an actual panel and make program resizeable (maybe)
    // 2. All those edit boxes should probably be classes...

    int panel_pos_x = 420;
    int panel_pos_y = 10;
    int edit_width = 35;
    int edit_caption_height = 17;
    int caption_width = 47;
    int caption_panel_pos_x = panel_pos_x + edit_width + 10;
    int y_space = 24;

    sens_box = CreateWindow(L"EDIT",
      L"4",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 0, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(sens_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    //SetWindowSubclass(sens_box,ButtonProc,0,0); // 

    accel_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 1, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(accel_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    offset_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 2, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(offset_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    senscap_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 3, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(senscap_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    power_box = CreateWindow(L"EDIT",
      L"2",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 4, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(power_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    m_cpi_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 5, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(m_cpi_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    fps_box = CreateWindow(L"EDIT",
      L"125",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 6, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(fps_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    x_scale_box = CreateWindow(L"EDIT",
      L"8",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 7, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(x_scale_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    y_scale_box = CreateWindow(L"EDIT",
      L"8",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      panel_pos_x, panel_pos_y + y_space * 8, edit_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(y_scale_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    caption_box = CreateWindow(L"STATIC",
      L"sens",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 0, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"cl_sensitivity");
    CreateToolTipForRect(sens_box, hInst, L"cl_sensitivity");

    caption_box = CreateWindow(L"STATIC",
      L"accel",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 1, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"cl_mouseAccel");
    CreateToolTipForRect(accel_box, hInst, L"cl_mouseAccel");

    caption_box = CreateWindow(L"STATIC",
      L"offset",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 2, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"cl_mouseAccelOffset");
    CreateToolTipForRect(offset_box, hInst, L"cl_mouseAccelOffset");

    caption_box = CreateWindow(L"STATIC",
      L"cap",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 3, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"cl_mouseSensCap");
    CreateToolTipForRect(senscap_box, hInst, L"cl_mouseSensCap");

    caption_box = CreateWindow(L"STATIC",
      L"power",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 4, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"cl_mouseAccelPower");
    CreateToolTipForRect(power_box, hInst, L"cl_mouseAccelPower");

    caption_box = CreateWindow(L"STATIC",
      L"m_cpi",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 5, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"m_cpi");
    CreateToolTipForRect(m_cpi_box, hInst, L"m_cpi");

    caption_box = CreateWindow(L"STATIC",
      L"fps",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 6, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"com_maxfps");
    CreateToolTipForRect(fps_box, hInst, L"com_maxfps");

    caption_box = CreateWindow(L"STATIC",
      L"x-scale",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 7, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"scale for x-axis");
    CreateToolTipForRect(x_scale_box, hInst, L"scale for x-axis");

    caption_box = CreateWindow(L"STATIC",
      L"y-scale",
      WS_CHILD | WS_VISIBLE | SS_NOTIFY,
      caption_panel_pos_x, panel_pos_y + y_space * 8, caption_width, edit_caption_height,
      hWnd, NULL, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(caption_box, hInst, L"scale for y-axis");
    CreateToolTipForRect(y_scale_box, hInst, L"scale for y-axis");


    text_box = CreateWindow(L"STATIC",
      L"If you can see this something is not working.",
      WS_CHILD | WS_VISIBLE,
      10, 220, 400, 30,
      hWnd, NULL, NULL, NULL);

    convert_button = CreateWindow(L"BUTTON",
      L"convert",
      WS_CHILD | WS_VISIBLE | BS_FLAT,
      432, 222, 55, 23,
      hWnd, (HMENU)IDC_CONVERT_BUTTON, NULL, NULL);
    SendMessage(convert_button, WM_SETFONT, (WPARAM)standard_font, TRUE);
    CreateToolTipForRect(convert_button, hInst, L"If m_cpi is set to 0, convert all settings to m_cpi system. Otherwise, convert all settings back.");

    SetWindowLongPtr(text_box, -4, (LONG_PTR)&TextBoxProc);
    break;
  }

  case WM_COMMAND:
    wmId = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    if (wmEvent == EN_CHANGE) {
      GetWindowText(sens_box, global_temp_string, 10);
      cvar_sens = wcstod(global_temp_string, NULL);
      GetWindowText(offset_box, global_temp_string, 10);
      cvar_offset = wcstod(global_temp_string, NULL);
      GetWindowText(accel_box, global_temp_string, 10);
      cvar_accel = wcstod(global_temp_string, NULL);
      GetWindowText(senscap_box, global_temp_string, 10);
      cvar_senscap = wcstod(global_temp_string, NULL);
      GetWindowText(power_box, global_temp_string, 10);
      cvar_power = wcstod(global_temp_string, NULL) - 1;
      if (cvar_power < 0) cvar_power = 0;
      GetWindowText(m_cpi_box, global_temp_string, 10);
      cvar_m_cpi = wcstod(global_temp_string, NULL);
      GetWindowText(fps_box, global_temp_string, 10);
      max_fps = wcstod(global_temp_string, NULL);
      GetWindowText(x_scale_box, global_temp_string, 10);
      graph_max_x = wcstod(global_temp_string, NULL);
      GetWindowText(y_scale_box, global_temp_string, 10);
      graph_max_y = wcstod(global_temp_string, NULL);
      InvalidateRect(hWnd, &invalidate_rect, false);
    }
    switch (wmId)
    {
    case ID_HELP_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, AboutProc);
      break;
    case ID_FILE_EXIT:
      DestroyWindow(hWnd);
      break;
    case IDC_CONVERT_BUTTON:
      if (cvar_power != 1){
        MessageBeep(1);
        DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG3), hWnd, ErrorProc);
      }
      else {
        if (cvar_m_cpi == 0) {
          DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG2), hWnd, ConvertProc);
        }
        else ConvertFromMcpi();
      }
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;

  case WM_INPUT: {

    UINT dwSize;
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize,
      sizeof(RAWINPUTHEADER));
    LPBYTE lpb = new BYTE[dwSize];
    if (lpb == NULL)
    {
      return 0;
    }

    if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize,
      sizeof(RAWINPUTHEADER)) != dwSize)
      OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

    RAWINPUT* raw = (RAWINPUT*)lpb;


    if (raw->header.dwType == RIM_TYPEMOUSE)
    {
      lastx += raw->data.mouse.lLastX;
      lasty += raw->data.mouse.lLastY;

      auto t_end = std::chrono::high_resolution_clock::now();
      double long_duration = (std::chrono::duration<double, std::micro>(t_end - t_start).count());
      double short_duration = (std::chrono::duration<double, std::micro>(t_end - t_start_short).count());
      double cur_mouse_poll_rate = 1000 * 1000 / short_duration;

      if ((double)long_duration + 1000 > 1000 * 1000 / (max_fps)){
        cur_fps = 1 * 1000 * 1000 / long_duration;
        double mcpi2 = cvar_m_cpi / 2.54;
        double usex = lastx;
        double usey = lasty;
        if (mcpi2 > 0) {
          usex = lastx / mcpi2;
          usey = lasty / mcpi2;
        }
        speed = sqrt((double)usex*usex + usey*usey);

        speed = 1000 * speed / long_duration;
        if (mcpi2 > 0) {
          speed = speed * 1000;
          speed_unit_str = speed_unit_str_cmps;
          move_unit_str = move_unit_str_cmps;
        }
        else {
          speed_unit_str = speed_unit_str_cpms;
          move_unit_str = move_unit_str_cpms;
        }
        hResult = StringCchPrintf(info_string_left, STRSAFE_MAX_CCH, TEXT("mouse speed: %.3f %s\r\nmultiplier: %.3f %s\r\n"),
          speed, speed_unit_str,
          multi, move_unit_str);
        hResult = StringCchPrintf(info_string_right, STRSAFE_MAX_CCH, TEXT("mouse poll rate: %04.0f Hz\r\n%.0f fps"),
          cur_mouse_poll_rate, cur_fps);

        InvalidateRect(text_box, &invalidate_rect_2, false);
        InvalidateRect(hWnd, &invalidate_rect, false);

        if (FAILED(hResult))
        {
          // TODO: write error handler
        }
        // OutputDebugString(info_string_left);
        // Static_SetText(text_box, info_string_left);
        

        t_start = t_end;
        lastx = 0;
        lasty = 0;
      }
      t_start_short = t_end;

    }
    delete[] lpb;

    return 0;
  }



  case WM_PAINT:

    hdc = BeginPaint(hWnd, &ps);

    hdc_backbuffer = CreateCompatibleDC(hdc);
    hbm_backbuffer = CreateCompatibleBitmap(hdc, graph_width, graph_height);

    h_old = SelectObject(hdc_backbuffer, hbm_backbuffer);

    FillRect(hdc_backbuffer, &invalidate_rect, black_brush);
    hResult = StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.1f [%s]"), graph_max_y, move_unit_str);
    SetTextColor(hdc_backbuffer, 0x00DDDDDD);
    SetBkColor(hdc_backbuffer, 0x00000000);
    SelectObject(hdc_backbuffer, standard_font);
    TextOut(hdc_backbuffer, 3, 0, global_temp_string, wcslen(global_temp_string));
    TextOut(hdc_backbuffer, 3, invalidate_rect.bottom - 27, L"0", 1);

    hResult = StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("[%s] %.1f"), speed_unit_str, graph_max_x);
    SetTextAlign(hdc_backbuffer, TA_RIGHT);
    TextOut(hdc_backbuffer, invalidate_rect.right - 13, invalidate_rect.bottom - 27, global_temp_string, wcslen(global_temp_string));


    //OutputDebugString(global_temp_string);


    for (int i = 0; i<graph_width; i++){
      double x = i*graph_max_x / graph_width;

      double y = cvar_sens + pow((cvar_accel*(x - cvar_offset)), cvar_power);

      if (cvar_offset>x) y = cvar_sens;
      if (y > cvar_senscap && cvar_senscap > 0) y = cvar_senscap;
      int j = graph_height - y*graph_height / graph_max_y;
      SetPixel(hdc_backbuffer, i, j, 0x00BBBBBB);
    }

    multi = cvar_sens + pow((cvar_accel*(speed - cvar_offset)), cvar_power);

    if (cvar_offset > speed) multi = cvar_sens;
    if (multi > cvar_senscap && cvar_senscap > 0) multi = cvar_senscap;
    last_pixel.top = graph_height - multi*graph_height / graph_max_y - 2;
    last_pixel.left = speed*graph_width / graph_max_x - 2;
    last_pixel.right = speed*graph_width / graph_max_x + 3;
    last_pixel.bottom = graph_height - multi*graph_height / graph_max_y + 3;

    FillRect(hdc_backbuffer, &last_pixel, red_brush);

    BitBlt(hdc, 10, 10, graph_width, graph_height, hdc_backbuffer, 0, 0, SRCCOPY);


    SelectObject(hdc_backbuffer, h_old);
    DeleteObject(hbm_backbuffer);
    DeleteDC(hdc_backbuffer);


    EndPaint(hWnd, &ps);

    break;
  case WM_CTLCOLORSTATIC:
  {
    HDC hdcStatic = (HDC)wParam;
    SetTextColor(hdcStatic, text_color);
    SetBkColor(hdcStatic, background_color);

    return (INT_PTR)background_brush;
  }
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  default:
    return DefWindowProc(hWnd, message, wParam, lParam);
  }
  return 0;
}

INT_PTR CALLBACK AboutProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
  case WM_CTLCOLORDLG:
    return (INT_PTR)background_brush;
  case WM_CTLCOLORSTATIC:
  {
    HDC hdcStatic = (HDC)wParam;
    SetTextColor(hdcStatic, text_color);
    SetBkColor(hdcStatic, background_color);

    return (INT_PTR)background_brush;
  }

  }
  return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ErrorProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
  case WM_CTLCOLORDLG:
    return (INT_PTR)background_brush;
  case WM_CTLCOLORSTATIC:
  {
    HDC hdcStatic = (HDC)wParam;
    SetTextColor(hdcStatic, text_color);
    SetBkColor(hdcStatic, background_color);

    return (INT_PTR)background_brush;
  }
  }
  return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ConvertProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
  UNREFERENCED_PARAMETER(lParam);
  switch (message)
  {
  case WM_INITDIALOG:
    mcpi_convert_box = CreateWindow(L"EDIT",
      L"",
      WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP,
      105, 10, 35, 17,
      hDlg, (HMENU)IDC_MCPI_BOX, NULL, NULL);
    SendMessage(mcpi_convert_box, WM_SETFONT, (WPARAM)standard_font, TRUE);

    caption_box = CreateWindow(L"STATIC",
      L"CPI",
      WS_CHILD | WS_VISIBLE,
      145, 12, 35, 17,
      hDlg, (HMENU)IDC_MCPI_BOX, NULL, NULL);
    SendMessage(caption_box, WM_SETFONT, (WPARAM)standard_font, TRUE);
    //SetFocus(GetDlgItem(hDlg, IDC_MCPI_BOX));
    SendMessage(hDlg, WM_NEXTDLGCTL, (WPARAM)mcpi_convert_box, TRUE);
    return (INT_PTR)FALSE;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK)
    {
      GetDlgItemText(hDlg, IDC_MCPI_BOX, global_temp_string, 10);
      double convert_to_mcpi = wcstod(global_temp_string, NULL);
      ConvertToMcpi(convert_to_mcpi);

      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    if (LOWORD(wParam) == IDCANCEL)
    {
      EndDialog(hDlg, LOWORD(wParam));
      return (INT_PTR)TRUE;
    }
    break;
  case WM_CTLCOLORDLG:
    return (INT_PTR)background_brush;
  case WM_CTLCOLORSTATIC:
  {
    HDC hdcStatic = (HDC)wParam;
    SetTextColor(hdcStatic, text_color);
    SetBkColor(hdcStatic, background_color);

    return (INT_PTR)background_brush;
  }
  }
  return (INT_PTR)FALSE;
}


void ConvertToMcpi(double convert_to_mcpi){
  double new_sens = SensToMcpiSens(cvar_sens, 0.022, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_sens);
  Static_SetText(sens_box, global_temp_string);

  double new_senscap = SensToMcpiSens(cvar_senscap, 0.022, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_senscap);
  Static_SetText(senscap_box, global_temp_string);

  double new_accel = AccelToMcpiAccel(cvar_accel, 0.022, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_accel);
  Static_SetText(accel_box, global_temp_string);

  double new_offset = OffsetToMcpiOffset(cvar_offset, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_offset);
  Static_SetText(offset_box, global_temp_string);

  double new_y_scale = SensToMcpiSens(graph_max_y, 0.022, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_y_scale);
  Static_SetText(y_scale_box, global_temp_string);

  double new_x_scale = SpeedToMcpiSpeed(graph_max_x, convert_to_mcpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_x_scale);
  Static_SetText(x_scale_box, global_temp_string);

  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.0f"), convert_to_mcpi);
  Static_SetText(m_cpi_box, global_temp_string);
}

void ConvertFromMcpi(){
  double new_sens = McpiSensToSens(cvar_sens, 0.022, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_sens);
  Static_SetText(sens_box, global_temp_string);

  double new_senscap = McpiSensToSens(cvar_senscap, 0.022, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_senscap);
  Static_SetText(senscap_box, global_temp_string);

  double new_accel = McpiAccelToAccel(cvar_accel, 0.022, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_accel);
  Static_SetText(accel_box, global_temp_string);

  double new_offset = McpiOffsetToOffset(cvar_offset, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_offset);
  Static_SetText(offset_box, global_temp_string);

  double new_y_scale = McpiSensToSens(graph_max_y, 0.022, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_y_scale);
  Static_SetText(y_scale_box, global_temp_string);

  double new_x_scale = McpiSpeedToSpeed(graph_max_x, cvar_m_cpi);
  StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.2f"), new_x_scale);
  Static_SetText(x_scale_box, global_temp_string);

  Static_SetText(m_cpi_box, L"0");
}