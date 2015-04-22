/*
KRNL386.EXEのヘッダ
*/
#include "wow32.h"
//15
HGLOBAL16 GlobalAlloc16(UINT16 flags, DWORD size);
//18
LPVOID16 GlobalLock16(HGLOBAL16 hMem);
//30
BOOL16 WaitEvent16(HTASK16 hTask);
//49
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName, INT16 nSize);
//51
FARPROC16 MakeProcInstance16(FARPROC16 lpProc, HINSTANCE16 hInst);
//52
void FreeProcInstance16(FARPROC16 lpProc);
//58
INT16 GetProfileString16(LPCSTR lpSect, LPCSTR lpKey, LPCSTR lpDefault, LPSTR lpReturn, INT16 nSize);
//レジスタ返し
//91
void InitTask16();
