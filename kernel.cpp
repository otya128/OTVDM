#include <windows.h>
#include "wow32.h"
#include "kernel.h"
//30
BOOL16 WaitEvent16(HTASK16 hTask)
{
	return 0;
}
HTASK16 CurrentTask = 0;//とりあえず
//36
HTASK16 GetCurrentTask16()
{
	return CurrentTask;
}
//49
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName, INT16 nSize)
{
	return 0;
}
//レジスタ返し
//91
void InitTask16()
{
	REG16(AX) = 0;//正常
}
