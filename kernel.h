/*
KRNL386.EXEのヘッダ
*/
#include "wow32.h"
//30
BOOL16 WaitEvent16(HTASK16 hTask);
//49
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName,INT16 nSize);
//レジスタ返し
//91
void InitTask16();
