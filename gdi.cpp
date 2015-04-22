#include "gdi.h"
//330
INT16 EnumFontFamilies16(HDC16 hdc, char *lpstrFontName, FONTENUMPROC16 lpfnFontEnumProc,
	LPARAM16 UserData)
{
	NOTIMPL("EnumFontFamilies(0x%X,\"%s\",0x%X,%d)\n", hdc, lpstrFontName, lpfnFontEnumProc, UserData);
	return 0;
}