/*
GDI.EXE header
*/
#include "wow32.h"
typedef FARPROC16 FONTENUMPROC16;
//330
INT16 EnumFontFamilies16(HDC16 hdc, char *lpstrFontName, FONTENUMPROC16 lpfnFontEnumProc,
LPARAM16 UserData);
