#include "user.h"
INT16 MessageBox16(HWND16 hWndParent, LPCSTR lpszMessage, LPCSTR lpszTitle, UINT16 uStyle)
{
	return MessageBoxA((HWND)HANDLE16ToHANDLE(hWndParent), lpszMessage, lpszTitle, uStyle);
}
UINT16 InitApp16(HINSTANCE16 hInstance)
{
	return TRUE;//なんか
}
//6
void PostQuitMessage16(INT16 nExitCode)
{
	PostQuitMessage(nExitCode);
}
void RECTToRECT16(RECT *from, RECT16 *to)
{
	to->left = (INT16)from->left;
	to->top = (INT16)from->top;
	to->right = (INT16)from->right;
	to->bottom = (INT16)from->bottom;
}
//32
void GetWindowRect16(HWND16 hWnd, RECT16 *RectPtr)
{
	HWND hWnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	RECT rect32;
	GetWindowRect(hWnd32, &rect32);
	RECTToRECT16(&rect32, RectPtr);
}
//33
void GetClientRect16(HWND16 hWnd, RECT16 *RectPtr)
{
	HWND hWnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	RECT rect32;
	GetClientRect(hWnd32, &rect32);
	RECTToRECT16(&rect32, RectPtr);
}
//41
HWND16 CreateWindow16(LPCSTR lpszClassName, LPCSTR lpszWindowName, DWORD dwStyle,
	INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu, HINSTANCE16 hInstance,
	LPVOID16 lpCreateParams)
{
	return CreateWindowEx16(0, lpszClassName, lpszWindowName, dwStyle,
		x, y, nWidth, nHeight, hWndParent, hMenu, hInstance,
		lpCreateParams);
}
HWND16 CreateWindowEx16(DWORD dwExStyle, LPCSTR lpszClassName, LPCSTR lpszWindowName,
	DWORD dwStyle, INT16 x, INT16 y, INT16 nWidth, INT16 nHeight, HWND16 hWndParent, HMENU16 hMenu,
	HINSTANCE16 hInstance, LPVOID16 lpCreateParams)
{
	HWND hWnd = CreateWindowExA(dwExStyle,
		lpszClassName,
		lpszWindowName,
		dwStyle,
		x == CW_USEDEFAULT16 ? CW_USEDEFAULT : x,
		y == CW_USEDEFAULT16 ? CW_USEDEFAULT : y,
		nWidth == CW_USEDEFAULT16 ? CW_USEDEFAULT : nWidth,
		nHeight == CW_USEDEFAULT16 ? CW_USEDEFAULT : nHeight,
		(HWND)HANDLE16ToHANDLE(hWndParent),
		(HMENU)HANDLE16ToHANDLE(hMenu),
		(HINSTANCE)HANDLE16ToHANDLE(hInstance),
		(LPVOID)lpCreateParams);
	return HANDLEToHANDLE16(hWnd);
}
#include <map>
//とりあえず
//TODO:ATOM実装したい
DWORD i86_galloca_ptr(void *ptr, WORD size);
void i86_gfree_ptr(WORD size);
extern std::map<std::string, DWORD> wprocmap;
LRESULT CALLBACK Win16WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	//dprintf("hwnd:%X,msg:%X,wp:%X,lp:%X,cs:%X,ip:%X\n", hwnd, msg, wp, lp);
	//return DefWindowProc(hwnd, msg, wp, lp);
	HWND16 hwnd16 = HANDLEToHANDLE16((HANDLE)hwnd);
	if (hwnd16)
	{
		//dprintf("wpp:%d,%X\n", inging, msg, msg, wp, lp);
		char name[256];
		//dprintf("%X\t", msg, msg, wp, lp);
		GetClassNameA(hwnd, name, sizeof(name));
		std::map<std::string, DWORD>::iterator itr;
		if ((itr = wprocmap.find(name)) != wprocmap.end())
		{/*
		 if (inging == 2)
		 {
		 inging = 3;
		 return DefWindowProc(hwnd, msg, wp, lp);
		 }*///if (inging) inging = 2;
			WORD cs, ip;
			ip = m_pc - m_base[SREGS::CS];
			cs = m_sregs[CS];
			UINT16 selector = itr->second >> 16, offset = (UINT16)itr->second;
			m_sregs[CS] = selector;
			m_base[CS] = selector << 4;
			m_rights[CS] = 0x9a;
			m_limit[CS] = 0xffff;
			UINT16 stk = m_regs.w[SP];
			int stkcnt = 0;
			switch (msg)
			{
			case WM_CREATE:
			{
				//lparam=LPCREATESTRUCT
				LPCREATESTRUCTA create = (LPCREATESTRUCTA)lp;
				CREATESTRUCT16 create16;//スタック上に確保
				create16.lpCreateParams = (LPVOID16)create->lpCreateParams;
				create16.hInstance = HANDLEToHANDLE16(create->hInstance);
				create16.hMenu = HANDLEToHANDLE16(create->hMenu);
				create16.hwndParent = HANDLEToHANDLE16(create->hwndParent);
				create16.cy = (INT16)create->cy;
				create16.cx = (INT16)create->cx;
				create16.y = (INT16)create->y;
				create16.x = (INT16)create->x;
				create16.style = (LONG32)create->style;
				//TODO:この値をずっと保持する場合互換性が保たれない
				create16.lpszName = i86_galloca_ptr((void*)create->lpszName, strlen(create->lpszName) + 2);
				stkcnt += strlen(create->lpszName) + 2;
				create16.lpszClass = i86_galloca_ptr((void*)create->lpszClass, strlen(create->lpszClass) + 2);
				stkcnt += strlen(create->lpszClass) + 2;
				create16.dwExStyle = create->dwExStyle;
				lp = i86_galloca_ptr(&create16, sizeof(CREATESTRUCT16));
				stkcnt += sizeof(CREATESTRUCT16);
			}
			break;
			case WM_SETCURSOR:
			case WM_MOUSEACTIVATE:
			case WM_ERASEBKGND:
			case WM_NCPAINT:
				wp = HANDLEToHANDLE16((HANDLE)wp);
				break;
			case WM_CAPTURECHANGED:
			case WM_SETICON:
				lp = HANDLEToHANDLE16((HANDLE)lp);
				break;
			}
			PUSH(hwnd16);
			PUSH(msg);
			PUSH(wp);
			PUSH(lp >> 16);
			PUSH(lp & 0xFFFF);
			PUSH(cs);
			PUSH(ip);
			m_pc = (m_base[CS] + offset)&AMASK;
			dprintf("hwnd:%X,msg:%X,wp:%X,lp:%X,cs:%X,ip:%X\n", hwnd16, msg, wp, lp, cs, ip);
			//CreateWindowの中でも呼ばれるので強引に
			//メッセージを溜めるようにした方がいいかもしれない
			while (m_regs.w[SP] < stk)
			{
				cpu_exexute_call_wrap();
			}
			i86_gfree_ptr(stkcnt);
			//dprintf("%X\n", m_pc, msg, wp, lp, cs, ip);
			return REG16(AX) | REG16(DX) << 16;
		}
	}
	return DefWindowProc(hwnd, msg, wp, lp);
}
ATOM16 RegisterClass16(const WNDCLASS16 *lpWndClass)
{
	WNDCLASSEXA wca;
	wca.cbSize = sizeof(wca);
	wca.style = lpWndClass->style;
	wca.lpfnWndProc = Win16WndProc;
	wca.hInstance = (HINSTANCE)HANDLE16ToHANDLE(lpWndClass->hInstance);
	wca.cbClsExtra = lpWndClass->cbClsExtra;
	wca.cbWndExtra = lpWndClass->cbWndExtra;
	wca.hIcon = (HICON)HANDLE16ToHANDLE(lpWndClass->hIcon);
	wca.hCursor = (HCURSOR)HANDLE16ToHANDLE(lpWndClass->hCursor);
	wca.hbrBackground = (HBRUSH)HANDLE16ToHANDLE(lpWndClass->hbrBackground);
	wca.lpszMenuName = (LPCSTR)FARPTRToPTR32(lpWndClass->lpszMenuName);
	wca.lpszClassName = (LPCSTR)FARPTRToPTR32(lpWndClass->lpszClassName);
	wca.hIconSm = NULL;
	wprocmap[wca.lpszClassName] = lpWndClass->lpfnWndProc;
	ATOM atom = RegisterClassExA(&wca);
	//convert atom table
	return atom;
}
LRESULT16 DefWindowProc16(HWND16 hWnd, UINT16 uMsg, WPARAM16 wParam, LPARAM16 lParam)
{
	HWND hwnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	WPARAM wParam32 = wParam;
	LPARAM lParam32 = lParam;
	switch (uMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT16 *create16 = (CREATESTRUCT16*)FARPTRToPTR32(lParam);
		CREATESTRUCTA create;
		create.lpCreateParams = (LPVOID)create16->lpCreateParams;
		create.hInstance = (HINSTANCE)HANDLE16ToHANDLE(create16->hInstance);
		create.hMenu = (HMENU)HANDLE16ToHANDLE(create16->hMenu);
		create.hwndParent = (HWND)HANDLE16ToHANDLE(create16->hwndParent);
		create.cy = create16->cy;
		create.cx = create16->cx;
		create.y = create16->y;
		create.x = create16->x;
		create.style = (LONG32)create16->style;
		create.lpszName = (LPCSTR)FARPTRToPTR32(create16->lpszName);
		create.lpszClass = (LPCSTR)FARPTRToPTR32(create16->lpszClass);
		create.dwExStyle = create16->dwExStyle;
		lParam32 = (LPARAM)&create;
	}
	break;
	case WM_SETCURSOR:
	case WM_MOUSEACTIVATE:
	case WM_ERASEBKGND:
	case WM_NCPAINT:
		wParam32 = (WPARAM)HANDLE16ToHANDLE(wParam);
		break;
	case WM_CAPTURECHANGED:
	case WM_SETICON:
		lParam32 = (LPARAM)HANDLE16ToHANDLE(lParam);
	}
	//TODO:result
	dprintf("hwnd:%X,msg:%X,wp:%X,lp:%X\n", hWnd, uMsg, wParam, lParam);
	return DefWindowProcA(hwnd32, uMsg, wParam32, lParam32);
}
BOOL16 GetMessage16(MSG16 *lpMsg, HWND16 hWnd, UINT16 uMsgFilterMin,
	UINT16 uMsgFilterMax)
{
	HWND hWnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	MSG msg32;
	BOOL16 result = GetMessageA(&msg32, hWnd32, uMsgFilterMin, uMsgFilterMax);
	lpMsg->hwnd = HANDLEToHANDLE16(msg32.hwnd);
	lpMsg->message = msg32.message;
	lpMsg->wParam = msg32.wParam;
	lpMsg->lParam = msg32.lParam;
	lpMsg->time = msg32.time;
	lpMsg->pt.x = msg32.pt.x;
	lpMsg->pt.y = msg32.pt.y;
	return result;
}
LONG16 DispatchMessage16(const MSG16 *lpMsg)
{
	MSG msg32;
	msg32.hwnd = (HWND)HANDLE16ToHANDLE(lpMsg->hwnd);
	msg32.message = lpMsg->message;
	msg32.wParam = lpMsg->wParam;
	msg32.lParam = lpMsg->lParam;
	msg32.time = lpMsg->time;
	msg32.pt.x = lpMsg->pt.x;
	msg32.pt.y = lpMsg->pt.y;
	return DispatchMessageA(&msg32);
}
//135
LONG16 GetWindowLong16(HWND16 hWnd, INT16 ByteOffset)
{
	HWND hWnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	if (ByteOffset == GWL_WNDPROC)
	{
		hWnd = hWnd;
	}
	return GetWindowLongA(hWnd32, ByteOffset);
}