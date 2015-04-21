/*
USER.EXE header
*/
#include "wow32.h"
typedef WORD ATOM16;
#define CW_USEDEFAULT16   ((INT16)0x8000)
typedef struct {
	INT16 left;
	INT16 top;
	INT16 right;
	INT16 bottom;
} RECT16;
//1
INT16 MessageBox16(HWND16 hWndParent, LPCSTR lpszMessage, LPCSTR lpszTitle, UINT16 uStyle);
//5
UINT16 InitApp16(HINSTANCE16 hInstance);
//6
void PostQuitMessage16(INT16 nExitCode);
//32
void GetWindowRect16(HWND16 hWnd, RECT16 *RectPtr);
//33
void GetClientRect16(HWND16 hWnd, RECT16 *RectPtr);
//41
HWND16 CreateWindow16(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle,
	INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu, HINSTANCE16 hInstance,
	LPVOID16 lpCreateParams);

HWND16 CreateWindowEx16(DWORD dwExStyle, LPCSTR lpszClassName, LPCSTR lpszWindowName,
	DWORD dwStyle, INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu,
	HINSTANCE16 hInstance, LPVOID16 lpCreateParams);
//57
ATOM16 RegisterClass16(const WNDCLASS16 *lpWndClass);
//107
LRESULT16 DefWindowProc16(HWND16 hWnd, UINT16 uMsg, WPARAM16 wParam, LPARAM16 lParam);
//108
BOOL16 GetMessage16(MSG16 *lpMsg, HWND16 hWnd, UINT16 uMsgFilterMin,
	UINT16 uMsgFilterMax);
//114
LONG16 DispatchMessage16(const MSG16 *lpMsg);
#define GWW_HINSTANCE   (-6)
#define GWW_HWNDPARENT  (-8)
#define GWW_ID          (-12)
//135
LONG16 GetWindowLong16(HWND16 hWnd, INT16 ByteOffset);
