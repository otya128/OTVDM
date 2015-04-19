#include <windows.h>
#include "wow32.h"
#include "kernel.h"
//30
BOOL16 WaitEvent16(HTASK16 hTask)
{

}
HTASK16 CurrentTask = 0;//Ç∆ÇËÇ†Ç¶Ç∏
//36
HTASK16 GetCurrentTask()
{
	return CurrentTask;
}
//49
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName, INT16 nSize)
{

}
//ÉåÉWÉXÉ^ï‘Çµ
//91
void InitTask16()
{
	REG16(AX) = 0;//ê≥èÌ
}
