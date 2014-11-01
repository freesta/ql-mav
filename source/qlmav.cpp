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

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND text_box = NULL;
HWND graph_box = NULL;
HWND sens_box= NULL;
HWND accel_box= NULL;
HWND offset_box= NULL;
HWND senscap_box= NULL;
HWND power_box= NULL;
HWND m_cpi_box = NULL;
HWND fps_box = NULL;
HWND x_scale_box = NULL;
HWND y_scale_box = NULL;
HWND controlcontainer = NULL;

double x_max = 8;
double y_max = 8;
long graph_x_max=400;
long graph_y_max=200;
double speed=0.0;
double multi=0.0;
double accel=3;
double senscap=5;
double sens=3;
double offset=0.4;
double power=2;
double m_cpi=400;
double fps=125;
long lastx=0;
long lasty=0;

RECT invalidate_rect={10,10,410,210};
BITMAP bitmap;
RECT last_pixel;
HBRUSH red=CreateSolidBrush(0x000000FF);
HBRUSH white=CreateSolidBrush(0x00FFFFFF);
HBRUSH black=CreateSolidBrush(0x00000000);
HDC          hdc_backbuffer;
HBITMAP      hbm_backbuffer;
HANDLE       h_old;

auto t_start = std::chrono::high_resolution_clock::now();

TCHAR global_temp_string[300];
TCHAR speed_unit_str_cpms[10]=L"count/ms";
TCHAR speed_unit_str_cmps[10]=L"cm/s";
TCHAR move_unit_str_cpms[21]=L"deg/(count*0.022)";
TCHAR move_unit_str_cmps[10]=L"deg/cm";
TCHAR *speed_unit_str=speed_unit_str_cpms;
TCHAR *move_unit_str=move_unit_str_cpms;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                       _In_opt_ HINSTANCE hPrevInstance,
                       _In_ LPTSTR    lpCmdLine,
                       _In_ int       nCmdShow)
{
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  // TODO: Place code here.
  MSG msg;
  HACCEL hAccelTable;

  // Initialize global strings
  LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
  LoadString(hInstance, IDC_QLMOUSEACCELHELPER, szWindowClass, MAX_LOADSTRING);
  MyRegisterClass(hInstance);

  // Perform application initialization:
  if (!InitInstance (hInstance, nCmdShow))
  {
    return FALSE;
  }

  hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QLMOUSEACCELHELPER));

  // Main message loop:
  while (GetMessage(&msg, NULL, 0, 0))
  {
    if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }

  return (int) msg.wParam;
}



ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style			= CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= WndProc;
  wcex.cbClsExtra		= 0;
  wcex.cbWndExtra		= 0;
  wcex.hInstance		= hInstance;
  //	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QLMOUSEACCELHELPER
  wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
  wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
  wcex.lpszMenuName	= MAKEINTRESOURCE(IDR_MENU1);
  wcex.lpszClassName	= szWindowClass;
  wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

  return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
  HWND hWnd;

  hInst = hInstance;

  //  hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
  //     CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
  hWnd = CreateWindow(szWindowClass, szTitle,  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
    CW_USEDEFAULT, CW_USEDEFAULT, 545, 360, NULL, NULL, hInstance, NULL);

  if (!hWnd)
  {
    return FALSE;
  }

  ShowWindow(hWnd, nCmdShow);
  SendMessage(hWnd,WM_COMMAND,MAKEWPARAM(0, EN_CHANGE), 0);
  UpdateWindow(hWnd);
  //   SetWindowPos(hWnd,HWND_TOPMOST,0,0,444,444,0);
  RAWINPUTDEVICE rid[1];

  rid[0].usUsagePage = 0x01; 
  rid[0].usUsage = 0x02; 
  rid[0].dwFlags = RIDEV_INPUTSINK;  
  rid[0].hwndTarget = hWnd;



  if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE) {
    //registration failed. Call GetLastError for the cause of the error
  }
  return TRUE;
}

// Tooltips are not used at the moment.
void CreateToolTipForRect(HWND hwndParent, HINSTANCE ginst, TCHAR* TEXT)
{
  // Create a tooltip.
  HWND hwndTT = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL, 
    WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, 
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
    hwndParent, NULL, ginst,NULL);

  SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, 
    SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

  // Set up "tool" information. In this case, the "tool" is the entire parent window.

  TOOLINFO ti = { 0 };
  ti.cbSize   = sizeof(TOOLINFO);
  ti.uFlags   = TTF_SUBCLASS;
  ti.hwnd     = hwndParent;
  ti.hinst    = ginst;
  ti.lpszText = TEXT("This is your tooltip string.");

  GetClientRect (hwndParent, &ti.rect);

  // Associate the tooltip with the "tool" window.
  SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM) (LPTOOLINFO) &ti);	
} 


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  int wmId, wmEvent;
  PAINTSTRUCT ps;
  HDC hdc;
  HRESULT hResult;




  switch (message)
  {
  case WM_GETMINMAXINFO:
    {
      LPMINMAXINFO p_info = (LPMINMAXINFO)lParam;
      int total_border_width = 2 * GetSystemMetrics( SM_CXFRAME );
      int total_border_height = 2 * GetSystemMetrics( SM_CYFRAME ) + 
        GetSystemMetrics( SM_CYCAPTION ) - GetSystemMetrics( SM_CYBORDER );
      POINT min,max;

      min.x = 545 > 0 ? 545+ total_border_width : p_info->ptMinTrackSize.x;
      min.y = 360 > 0 ? 360 + total_border_height : p_info->ptMinTrackSize.y;
      max.x = 545 > 0 ? 545  + total_border_width : p_info->ptMaxTrackSize.x;
      max.y = 360 > 0 ? 360  + total_border_height : p_info->ptMaxTrackSize.y;

      p_info->ptMinTrackSize = min;
      p_info->ptMaxTrackSize = max;
    }

    break;
  case WM_CREATE:
    sens_box = CreateWindow(L"EDIT",
      L"3",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,10,45,20,
      hWnd, NULL,NULL, NULL);
    accel_box = CreateWindow(L"EDIT",
      L"0.1",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,40,45,20,
      hWnd, NULL,NULL, NULL);
    offset_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,70,45,20,
      hWnd, NULL,NULL, NULL);
    senscap_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,100,45,20,
      hWnd, NULL,NULL, NULL);
    power_box = CreateWindow(L"EDIT",
      L"2",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,130,45,20,
      hWnd, NULL,NULL, NULL);
    m_cpi_box = CreateWindow(L"EDIT",
      L"0",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,160,45,20,
      hWnd, NULL,NULL, NULL);
    fps_box = CreateWindow(L"EDIT",
      L"125",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,190,45,20,
      hWnd, NULL,NULL, NULL);
    x_scale_box = CreateWindow(L"EDIT",
      L"8",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,220,45,20,
      hWnd, NULL,NULL, NULL);
    y_scale_box = CreateWindow(L"EDIT",
      L"8",
      WS_BORDER | WS_CHILD | WS_VISIBLE,
      420,250,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"sens",
      WS_CHILD | WS_VISIBLE,
      475,10,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"accel",
      WS_CHILD | WS_VISIBLE,
      475,40,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"offset",
      WS_CHILD | WS_VISIBLE,
      475,70,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"cap",
      WS_CHILD | WS_VISIBLE,
      475,100,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"power",
      WS_CHILD | WS_VISIBLE,
      475,130,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"m_cpi",
      WS_CHILD | WS_VISIBLE,
      475,160,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"fps",
      WS_CHILD | WS_VISIBLE,
      475,190,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"xScale",
      WS_CHILD | WS_VISIBLE,
      475,220,45,20,
      hWnd, NULL,NULL, NULL);
    CreateWindow(L"STATIC",
      L"yScale",
      WS_CHILD | WS_VISIBLE,
      475,250,45,20,
      hWnd, NULL,NULL, NULL);
    text_box = CreateWindow(L"STATIC",
      L"If you can see this something is not working.",
      WS_CHILD | WS_VISIBLE,
      10,220,400,100,
      hWnd, NULL,NULL, NULL);
    break;

  case WM_COMMAND:
    wmId    = LOWORD(wParam);
    wmEvent = HIWORD(wParam);
    if (wmEvent == EN_CHANGE) {
      GetWindowText(sens_box,global_temp_string,10);
      sens=wcstod(global_temp_string,NULL);
      GetWindowText(offset_box,global_temp_string,10);
      offset=wcstod(global_temp_string,NULL);
      GetWindowText(accel_box,global_temp_string,10);
      accel=wcstod(global_temp_string,NULL);
      GetWindowText(senscap_box,global_temp_string,10);
      senscap=wcstod(global_temp_string,NULL);
      GetWindowText(power_box,global_temp_string,10);
      power=wcstod(global_temp_string,NULL)-1;
      if (power<0) power=0;
      GetWindowText(m_cpi_box,global_temp_string,10);
      m_cpi=wcstod(global_temp_string,NULL);
      GetWindowText(fps_box,global_temp_string,10);
      fps=wcstod(global_temp_string,NULL);
      GetWindowText(x_scale_box,global_temp_string,10);
      x_max=(long)wcstod(global_temp_string,NULL);
      GetWindowText(y_scale_box,global_temp_string,10);
      y_max=(long)wcstod(global_temp_string,NULL);
    }
    switch (wmId)
    {
    case ID_HELP_ABOUT:
      DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
      break;
    case ID_FILE_EXIT:
      DestroyWindow(hWnd);
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
      sizeof(RAWINPUTHEADER)) != dwSize )
      OutputDebugString (TEXT("GetRawInputData does not return correct size !\n")); 

    RAWINPUT* raw = (RAWINPUT*)lpb;


    if (raw->header.dwType == RIM_TYPEMOUSE) 
    {    
      lastx += raw->data.mouse.lLastX;
      lasty += raw->data.mouse.lLastY;

      auto t_end = std::chrono::high_resolution_clock::now();
      double duration =(std::chrono::duration<double, std::micro>(t_end-t_start).count());

      if ((double)duration+1000>1000*1000/(fps)){
        double cur_fps=1*1000*1000/duration;
        double mcpi2=m_cpi/2.54;
        double usex=lastx;
        double usey=lasty;
        if (mcpi2>0) {
          usex=lastx/mcpi2;
          usey=lasty/mcpi2;
        } 
        speed = sqrt((double)usex*usex+usey*usey);

        speed= 1000*speed /duration;
        if (mcpi2>0) {
          speed=speed*1000;
          speed_unit_str=speed_unit_str_cmps;
          move_unit_str=move_unit_str_cmps;
        } else {
          speed_unit_str=speed_unit_str_cpms;
          move_unit_str=move_unit_str_cpms;
        }
        hResult = StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("mouse_speed=%.3f %s\r\nend_sensitivity=%.3f %s\r\nfps=%.0f\r\nRaw mouse data: \r\nlastX=%d lastY=%d \r\n"), 
          speed, speed_unit_str,
          multi, move_unit_str,
          cur_fps,
          lastx,
          lasty);

        InvalidateRect(hWnd,&invalidate_rect,false);

        if (FAILED(hResult))
        {
          // TODO: write error handler
        }
        // OutputDebugString(global_temp_string);
        Static_SetText(text_box, global_temp_string);
        t_start =t_end;
        lastx=0;
        lasty=0;
      }
    } 
    delete[] lpb;

    return 0;
                 }



  case WM_PAINT:

    hdc = BeginPaint(hWnd, &ps);

    hdc_backbuffer = CreateCompatibleDC(hdc);
    hbm_backbuffer = CreateCompatibleBitmap(hdc, graph_x_max, graph_y_max);

    h_old   = SelectObject(hdc_backbuffer, hbm_backbuffer);

    FillRect(hdc_backbuffer,&invalidate_rect,black);
    hResult = StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("%.0f (%s)"), y_max, move_unit_str);
    SetTextColor(hdc_backbuffer,0x00BBBBBB);
    SetBkColor(hdc_backbuffer,0x00000000);
    TextOut(hdc_backbuffer, 0, 0, global_temp_string, wcslen(global_temp_string));
    TextOut(hdc_backbuffer, 0, invalidate_rect.bottom-27, L"0", 1);

    hResult = StringCchPrintf(global_temp_string, STRSAFE_MAX_CCH, TEXT("(%s) %.0f"),  speed_unit_str, x_max);
    SetTextAlign(hdc_backbuffer,TA_RIGHT);
    TextOut(hdc_backbuffer, invalidate_rect.right-10, invalidate_rect.bottom-27, global_temp_string, wcslen(global_temp_string));



    OutputDebugString(global_temp_string);


    for (int i=0;i<graph_x_max;i++){
      double x=i*x_max/graph_x_max;

      double y=sens+pow((accel*(x-offset)),power);

      if (offset>x) y=sens;
      if (y>senscap && senscap>0) y=senscap;
      int j=graph_y_max-y*graph_y_max/y_max;
      SetPixel(hdc_backbuffer,i,j,0x00999999);
    }

    multi=sens+pow((accel*(speed-offset)),power);

    if (offset>speed) multi=sens;
    if (multi>senscap && senscap>0) multi=senscap;
    last_pixel.top=graph_y_max-multi*graph_y_max/y_max-2;
    last_pixel.left=speed*graph_x_max/x_max-2;
    last_pixel.right=speed*graph_x_max/x_max+3;
    last_pixel.bottom=graph_y_max-multi*graph_y_max/y_max+3;

    FillRect(hdc_backbuffer,&last_pixel,red);

    BitBlt(hdc, 10, 10, graph_x_max, graph_y_max, hdc_backbuffer, 0, 0, SRCCOPY);


    SelectObject(hdc_backbuffer, h_old);
    DeleteObject(hbm_backbuffer);
    DeleteDC    (hdc_backbuffer);


    EndPaint(hWnd,&ps);

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
