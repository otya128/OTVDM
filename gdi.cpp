#include "gdi.h"
DWORD call_func_pascal_32(DWORD farfunc, WORD size, BYTE* ptr)
{
	WORD cs, ip;
	ip = m_pc - m_base[SREGS::CS];
	cs = m_sregs[CS];
	UINT16 selector = farfunc >> 16, offset = (UINT16)farfunc;
	m_sregs[CS] = selector;
	m_base[CS] = selector << 4;
	m_rights[CS] = 0x9a;
	m_limit[CS] = 0xffff;
	UINT16 stk = m_regs.w[SP];
	WORD sizew = size >> 1;
	WORD *ptrw = (WORD*)ptr;
	for (int i = 0; i < sizew; i++)
	{
		dprintf("%04X", *ptrw);
		PUSH(*ptrw++);
		}/*
	for (int i = 0; i < size; i++)
	{
		dprintf("%02X", *ptr);
		PUSHB(*ptr++);
	}*/
	PUSH(cs);
	PUSH(ip);
	m_pc = (m_base[CS] + offset)&AMASK;
	while (m_regs.w[SP] < stk)
	{
		dprintf("%X,m:%02X,a:%04X,next->", m_pc, mem[m_pc],REG16(AX));
		cpu_exexute_call_wrap();
	}
	return REG16(AX) | REG16(DX) << 16;
}
WORD call_func_pascal_16(DWORD farfunc, WORD size, BYTE* ptr)
{
	return (WORD)call_func_pascal_32(farfunc, size, ptr);
}
WORD vcall_func_pascal_16(DWORD farfunc, WORD size,...)
{
	va_list ap;
	va_start(ap, size);
	WORD ret = (WORD)call_func_pascal_32(farfunc, size, (BYTE*)ap);
	va_end(ap);
	return ret;
}
DWORD WIN16_FontEnumProc = 0;
void LOGFONTToLOGFONT16(const LOGFONT *from, LOGFONT16 *to)
{
	to->lfHeight = from->lfHeight;
	to->lfWidth = from->lfWidth;
	to->lfEscapement = from->lfEscapement;
	to->lfOrientation = from->lfOrientation;
	to->lfWeight = from->lfWeight;
	to->lfItalic = from->lfItalic;
	to->lfUnderline = from->lfUnderline;
	to->lfStrikeOut = from->lfStrikeOut;
	to->lfCharSet = from->lfCharSet;
	to->lfOutPrecision = from->lfOutPrecision;
	to->lfClipPrecision = from->lfClipPrecision;
	to->lfQuality = from->lfQuality;
	to->lfPitchAndFamily = from->lfPitchAndFamily;
	memcpy(&to->lfFaceName, &from->lfFaceName, sizeof(to->lfFaceName));
}
void TEXTMETRICToTEXTMETRIC16(const TEXTMETRIC *from, TEXTMETRIC16 *to)
{
	to->tmHeight = from->tmHeight;
	to->tmAscent = from->tmAscent;
	to->tmDescent = from->tmDescent;
	to->tmInternalLeading = from->tmInternalLeading;
	to->tmExternalLeading = from->tmExternalLeading;
	to->tmAveCharWidth = from->tmAveCharWidth;
	to->tmMaxCharWidth = from->tmMaxCharWidth;
	to->tmWeight = from->tmWeight;
	to->tmItalic = from->tmItalic;
	to->tmUnderlined = from->tmUnderlined;
	to->tmStruckOut = from->tmStruckOut;
	to->tmFirstChar = from->tmFirstChar;
	to->tmLastChar = from->tmLastChar;
	to->tmDefaultChar = from->tmDefaultChar;
	to->tmBreakChar = from->tmBreakChar;
	to->tmPitchAndFamily = from->tmPitchAndFamily;
	to->tmCharSet = from->tmCharSet;
	to->tmOverhang = from->tmOverhang;
	to->tmDigitizedAspectX = from->tmDigitizedAspectX;
	to->tmDigitizedAspectY = from->tmDigitizedAspectY;
}
//330
int CALLBACK Win16FontEnumProc(CONST LOGFONTA *logfont, CONST TEXTMETRICA *textmetrica, DWORD fonttype, LPARAM data)
{
	if (!WIN16_FontEnumProc) return 0;
	LOGFONT16 logfont16;
	TEXTMETRIC16 textmetric16;
	LOGFONTToLOGFONT16(logfont, &logfont16);
	TEXTMETRICToTEXTMETRIC16(textmetrica, &textmetric16);
	DWORD logfontptr = i86_galloca_ptr((void*)&logfont16, sizeof(LOGFONT16));
	DWORD textmetricaptr = i86_galloca_ptr((void*)&textmetric16, sizeof(TEXTMETRIC16));

	WORD cs, ip;
	ip = m_pc - m_base[SREGS::CS];
	cs = m_sregs[CS];
	UINT16 selector = WIN16_FontEnumProc >> 16, offset = (UINT16)WIN16_FontEnumProc;
	m_sregs[CS] = selector;
	m_base[CS] = selector << 4;
	m_rights[CS] = 0x9a;
	m_limit[CS] = 0xffff;
	UINT16 stk = m_regs.w[SP];
	PUSH(logfontptr >> 16);
	PUSH(logfontptr & 0xFFFF);
	PUSH(textmetricaptr >> 16);
	PUSH(textmetricaptr & 0xFFFF);
	PUSH(fonttype);
	PUSH(data >> 16);
	PUSH(data & 0xFFFF);
	PUSH(cs);
	PUSH(ip);
	m_pc = (m_base[CS] + offset)&AMASK;
	while (m_regs.w[SP] < stk)
	{
		cpu_exexute_call_wrap();
	}
	WORD ret = REG16(AX);


	/*
	BYTE args[sizeof(DWORD) + sizeof(DWORD) + sizeof(UINT16) + sizeof(LPARAM)];
	BYTE *argp = (BYTE*)&args;
	*(DWORD*)argp = logfontptr;
	argp += sizeof(DWORD);
	*(DWORD*)argp = textmetricaptr;
	argp += sizeof(DWORD);
	*(UINT16*)argp = fonttype;
	argp += sizeof(UINT16);
	*(LPARAM*)argp = data;
	argp += sizeof(LPARAM);
	WORD ret = call_func_pascal_16(WIN16_FontEnumProc, sizeof(DWORD) + sizeof(DWORD) + sizeof(UINT16) + sizeof(LPARAM),
		(BYTE*)&args);*/
	i86_gfree_ptr(sizeof(LOGFONT16) + sizeof(TEXTMETRIC16));
	return ret;
}
INT16 EnumFontFamilies16(HDC16 hdc, char *lpstrFontName, FONTENUMPROC16 lpfnFontEnumProc,
	LPARAM16 UserData)
{
	NOTIMPL("EnumFontFamilies(0x%X,\"%s\",0x%X,%d)\n", hdc, lpstrFontName, lpfnFontEnumProc, UserData);
	WIN16_FontEnumProc = lpfnFontEnumProc;
	//FONTENUMPROCA
	EnumFontFamiliesA((HDC)HANDLE16ToHANDLE(hdc), lpstrFontName, Win16FontEnumProc, UserData);
	//WIN16_FontEnumProc = 0;
	return 0;
}
