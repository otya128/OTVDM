/*
USER.EXE header
*/
#include "wow32.h"
typedef WORD ATOM16;
#define CW_USEDEFAULT16   ((INT16)0x8000)
INT16 MessageBox16(HWND16 hWndParent, LPCSTR lpszMessage, LPCSTR lpszTitle, UINT16 uStyle);
UINT16 InitApp16(HINSTANCE16 hInstance);
void PostQuitMessage16(INT16 nExitCode);
HWND16 CreateWindow16(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle,
	INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu, HINSTANCE16 hInstance,
	LPVOID16 lpCreateParams);
HWND16 CreateWindowEx16(DWORD dwExStyle, LPCSTR lpszClassName, LPCSTR lpszWindowName,
	DWORD dwStyle, INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu,
	HINSTANCE16 hInstance, LPVOID16 lpCreateParams);
ATOM16 RegisterClass16(const WNDCLASS16 *lpWndClass);
LRESULT16 DefWindowProc16(HWND16 hWnd, UINT16 uMsg, WPARAM16 wParam, LPARAM16 lParam);
BOOL16 GetMessage16(MSG16 *lpMsg, HWND16 hWnd, UINT16 uMsgFilterMin,
	UINT16 uMsgFilterMax);
LONG16 DispatchMessage16(const MSG16 *lpMsg);
