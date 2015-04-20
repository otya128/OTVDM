#ifndef __WOW32__H__
#define __WOW32__H__
#include "win16.h"
union i80286basicregs
{                   /* eight general registers */
	UINT16 w[8];    /* viewed as 16 bits registers */
	UINT8  b[16];   /* or as 8 bit registers */
};
extern i80286basicregs m_regs;
extern UINT32  m_amask;          /* address mask */
extern UINT32  m_pc;
extern UINT8   m_prefix_seg;
extern unsigned m_ea;
extern UINT16 m_eo; /* HJB 12/13/98 effective offset of the address (before segment is added) */
extern UINT8 m_ea_seg;   /* effective segment of the address */
extern UINT32  m_base[4];
extern UINT16  m_sregs[4];
#if defined(HAS_I386)
#define SREG(x)				m_sreg[x].selector
#define SREG_BASE(x)			m_sreg[x].base

int cpu_type, cpu_step;
#else
#define REG8(x)				m_regs.b[x]
#define REG16(x)			m_regs.w[x]
#define SREG(x)				m_sregs[x]
#define SREG_BASE(x)			m_base[x]
#define m_CF				m_CarryVal
#define m_a20_mask			AMASK
#define i386_load_segment_descriptor(x)	m_base[x] = SegBase(x)
#if defined(HAS_I286)
#define i386_set_a20_line(x)	i80286_set_a20_line(x)
#else
#define i386_set_a20_line(x)
#endif
#define i386_set_irq_line(x, y)		set_irq_line(x, y)
#endif
// endian-based value: first value is if 'endian' is little-endian, second is if 'endian' is big-endian
#define ENDIAN_VALUE_LE_BE(endian,leval,beval)	(((endian) == ENDIANNESS_LITTLE) ? (leval) : (beval))

// endian-based value: first value is if native endianness is little-endian, second is if native is big-endian
#define NATIVE_ENDIAN_VALUE_LE_BE(leval,beval)	ENDIAN_VALUE_LE_BE(ENDIANNESS_NATIVE, leval, beval)

// endian-based value: first value is if 'endian' matches native, second is if 'endian' doesn't match native
#define ENDIAN_VALUE_NE_NNE(endian,leval,beval)	(((endian) == ENDIANNESS_NATIVE) ? (neval) : (nneval))
/*****************************************************************************/
/* src/emu/emucore.h */

// constants for expression endianness
enum endianness_t
{
	ENDIANNESS_LITTLE,
	ENDIANNESS_BIG
};

// declare native endianness to be one or the other
#ifdef LSB_FIRST
const endianness_t ENDIANNESS_NATIVE = ENDIANNESS_LITTLE;
#else
const endianness_t ENDIANNESS_NATIVE = ENDIANNESS_BIG;
#endif

#include "mame/emu/cpu/i86/i86priv.h"
typedef enum : BYTE
{
	SOURCE_MASK = 0xF,
	LOBYTE = 0x00,
	SEGMENT = 2,
	FAR_ADDR = 3,
	OFFSET = 5,
} reloctype;
typedef enum : BYTE
{
	TARGET_MASK = 0x3,
	INTERNALREF = 0x0,
	IMPORTORDINAL = 0x1,
	IMPORTNAME = 0x2,
	OSFIXUP = 0x3,
	ADDITIVE = 0x4,
} relocflag;
typedef struct
{
	reloctype type;
	relocflag flag;
	WORD offset;
	union
	{
		struct
		{
			BYTE segnum;
			BYTE _;
			WORD offset;
		} internalref;

		struct
		{
			WORD module;
			WORD name;
		} importname;

		struct
		{
			WORD module;
			WORD ordinal;
		} importordinal;
	};
} reloctable;
#define MAX_MEM 0x1000000
extern UINT8 mem[MAX_MEM + 3];
#define NOTIMPL dprintf("noimpl:");dprintf
typedef WORD HANDLE16;
typedef HANDLE16 HTASK16;
typedef HANDLE16 HINSTANCE16;
typedef HANDLE16 HWND16;
typedef HANDLE16 HGDIOBJ16;
typedef HANDLE16 HICON16;
typedef HANDLE16 HCURSOR16;
typedef HANDLE16 HBRUSH16;
typedef HANDLE16 HMENU16;
typedef unsigned short  BOOL16;
typedef DWORD           SEGPTR;
typedef DWORD           LPVOID16;
typedef DWORD           LPCSTR16;
typedef DWORD WNDPROC16;//far pascal
typedef UINT16 WPARAM16;
typedef INT32 LPARAM16;
typedef INT32 LRESULT16;
typedef INT32 LONG16;
#pragma pack(1)
typedef struct
{
	UINT16      style;
	WNDPROC16   lpfnWndProc;
	INT16       cbClsExtra;
	INT16       cbWndExtra;
	HANDLE16    hInstance;
	HICON16     hIcon;
	HCURSOR16   hCursor;
	HBRUSH16    hbrBackground;
	SEGPTR      lpszMenuName;
	SEGPTR      lpszClassName;
} WNDCLASS16, *LPWNDCLASS16;
typedef struct {
	INT16 x;
	INT16 y;
} POINT16;
typedef struct {
	HWND16    hwnd;
	UINT16    message;
	WPARAM16  wParam;
	LPARAM16  lParam;
	DWORD   time;
	POINT16   pt;
} MSG16;
typedef struct {
	LPVOID16    lpCreateParams;
	HINSTANCE16 hInstance;
	HMENU16     hMenu;
	HWND16      hwndParent;
	INT16       cy;
	INT16       cx;
	INT16       y;
	INT16       x;
	LONG16      style;
	LPCSTR16   lpszName;
	LPCSTR16   lpszClass;
	DWORD     dwExStyle;
} CREATESTRUCT16;
#pragma pack()
//ïœä∑ä÷êî
HANDLE16 AllocHANDLE16();
HANDLE HANDLE16ToHANDLE(HANDLE16 handle16);
HANDLE16 HANDLEToHANDLE16(HANDLE handle);
void *FARPTRToPTR32(DWORD farptr);
typedef struct
{
	HINSTANCE16 hInst;
} TASK16;
extern HTASK16 CurrentTask;
typedef struct
{
	HANDLE16 id;
	HANDLE16 next;
	DWORD data;
} HANDLE16Alloc;
extern HANDLE16Alloc HANDLE16array[65536];
#endif
