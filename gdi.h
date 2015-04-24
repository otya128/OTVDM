/*
GDI.EXE header
*/
#include "wow32.h"
typedef FARPROC16 FONTENUMPROC16;
#pragma pack(push)
#pragma pack(1)
#define LF_FACESIZE16             32
typedef struct tagLOGFONT16 {
	INT16     lfHeight;
	INT16     lfWidth;
	INT16     lfEscapement;
	INT16     lfOrientation;
	INT16     lfWeight;
	BYTE    lfItalic;
	BYTE    lfUnderline;
	BYTE    lfStrikeOut;
	BYTE    lfCharSet;
	BYTE    lfOutPrecision;
	BYTE    lfClipPrecision;
	BYTE    lfQuality;
	BYTE    lfPitchAndFamily;
	char    lfFaceName[LF_FACESIZE16];
} LOGFONT16;
typedef struct tagTEXTMETRIC16 {
	INT16     tmHeight;
	INT16     tmAscent;
	INT16     tmDescent;
	INT16     tmInternalLeading;
	INT16     tmExternalLeading;
	INT16     tmAveCharWidth;
	INT16     tmMaxCharWidth;
	INT16     tmWeight;
	BYTE    tmItalic;
	BYTE    tmUnderlined;
	BYTE    tmStruckOut;
	BYTE    tmFirstChar;
	BYTE    tmLastChar;
	BYTE    tmDefaultChar;
	BYTE    tmBreakChar;
	BYTE    tmPitchAndFamily;
	BYTE    tmCharSet;
	INT16     tmOverhang;
	INT16     tmDigitizedAspectX;
	INT16     tmDigitizedAspectY;
} TEXTMETRIC16;
#pragma pack(pop)
//330
INT16 EnumFontFamilies16(HDC16 hdc, char *lpstrFontName, FONTENUMPROC16 lpfnFontEnumProc,
LPARAM16 UserData);
