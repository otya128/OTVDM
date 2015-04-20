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
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName,INT16 nSize);
//レジスタ返し
//91
void InitTask16();
