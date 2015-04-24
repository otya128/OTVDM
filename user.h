/*
USER.EXE header
*/
#include "wow32.h"
#include <map>
class case_insensitive_string : std::string
{
public:
	bool operator < (const case_insensitive_string&rhs) const;
	bool operator > (const case_insensitive_string&rhs) const;
	bool operator <= (const case_insensitive_string&rhs) const;
	bool operator >= (const case_insensitive_string&rhs) const;
	bool operator == (const case_insensitive_string&rhs) const;
	case_insensitive_string(const char* c);
};
extern std::map<case_insensitive_string, HMENU16> resource_menu;
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
//42
BOOL16 ShowWindow16(HWND16 hWnd, INT16 nCmdShow);
//53
BOOL16 DestroyWindow16(HWND16 hWnd);
//56
BOOL16 MoveWindow16(HWND16 hWnd, INT16 LeftPos, INT16 TopPos, INT16 Width, INT16 Height, BOOL16 bRepaint);
//57
ATOM16 RegisterClass16(const WNDCLASS16 *lpWndClass);
//66
HDC16 GetDC16(HWND16 hWnd);
//68
BOOL16 ReleaseDC16(HWND16 hWnd, HDC16 hdc);
//107
LRESULT16 DefWindowProc16(HWND16 hWnd, UINT16 uMsg, WPARAM16 wParam, LPARAM16 lParam);
//108
BOOL16 GetMessage16(MSG16 *lpMsg, HWND16 hWnd, UINT16 uMsgFilterMin,
	UINT16 uMsgFilterMax);
//113
BOOL16 TranslateMessage16(const MSG16 *lpMsg);
//114
LONG16 DispatchMessage16(const MSG16 *lpMsg);
//124
void UpdateWindow16(HWND16 hwnd);
#define GWW_HINSTANCE   (-6)
#define GWW_HWNDPARENT  (-8)
#define GWW_ID          (-12)
//135
LONG16 GetWindowLong16(HWND16 hWnd, INT16 ByteOffset);
//136
LONG16 SetWindowLong16(HWND16 hWnd, INT16 ByteOffset, LONG16 Value);
//173
HCURSOR16 LoadCursor16(HINSTANCE16 hInstance, LPCSTR pszName);
//174
HICON16 LoadIcon16(HINSTANCE16 hInstance, LPCSTR ResourceID);
