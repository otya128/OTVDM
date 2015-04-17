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
bool isNE(const char* file)
{
	PIMAGE_DOS_HEADER EXE = (PIMAGE_DOS_HEADER)file;
	PIMAGE_OS2_HEADER NE = (PIMAGE_OS2_HEADER)(file + EXE->e_lfanew);
	if (NE->ne_magic != IMAGE_OS2_SIGNATURE)
	{
		return false;
	}
	return true;
}
int win16main(int argc, char *argv[], char *envp[])
{
	if (argc < 2)
	{
		fprintf(stderr, "OTVDM\n");
		fprintf(stderr, "Usage:%s command line\n", argv[0]);
		if (!is_started_from_command_prompt()) {
			fprintf(stderr, "\nStart this program from a command prompt!\n\nHit any key to quit...");
			while (!_kbhit()) {
				Sleep(10);
			}
		}
		return 1;
	}
	if (loadne(argv))
	{

	}
	if (!is_started_from_command_prompt()) {
		while (!_kbhit()) {
			Sleep(10);
		}
	}
	return 0;
}
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

#define MAX_MEM 0x1000000
extern UINT8 mem[MAX_MEM + 3];
#define NOTIMPL dprintf("noimpl:");dprintf
//moduleの関数を呼び出す
//とりあえず割り込み
//PUSH AX
//50
//MOV AX,num
//B8 L H
//INT WIN16_CALL
//CD L
//POP AX
//58
//6byte
//やめた
//INT WIN16_CALL
//CD L
//ここにデータを書く(2byte)(4byte?)
//RETはこっち側で処理
//引数月RETは引数BYTEスタックをpopする.
char **modtable;
typedef void(*modulehandler)();
void KERNEL_call(WORD ordinal);
modulehandler kernel_table[256];
int win16_init();
int _win16_init = win16_init();
void win16_call_module()
{
	WORD module = *(WORD*)(mem + m_pc);
	WORD ordinal = *(WORD*)(mem + m_pc + 2);
	if (!strcmp(modtable[module], "KERNEL"))
	{
		if (!kernel_table[ordinal])
			KERNEL_call(ordinal);
		else
			kernel_table[ordinal]();
		return;
	}/*
	if (!strcmp(modtable[module], "USER"))
	{
		if (!kernel_table[ordinal])
			KERNEL_call(ordinal);
		else
			kernel_table[ordinal]();
	}*/
	NOTIMPL("undefined %s function:%d\n", modtable[module], ordinal);
}
void KERNEL_call(WORD ordinal)
{
	switch (ordinal)
	{
	default:
		NOTIMPL("undefined KERNEL function:%d\n", ordinal);
		break;
	}
}
void InitTask16()
{
	REG16(AX) = 1;
}
int win16_init()
{
	int len = sizeof(kernel_table) / sizeof(modulehandler);
	kernel_table[91] = InitTask16;
	return 0;
}
//segment* load_segmentable(const char *file, int length);
//cs:ip,ss:sp
//moduleの関数呼び出し
//とりあえず1000:あたりに置いてみる
//module tableをつくる
//面倒だからとりあえず重複ありで考える
void dos_loadne(UINT8 *file, UINT16 *cs, UINT16 *ss, UINT16 *ip, UINT16 *sp, UINT8 *mem)
{
	PIMAGE_DOS_HEADER EXE = (PIMAGE_DOS_HEADER)file;
	PIMAGE_OS2_HEADER NE = (PIMAGE_OS2_HEADER)(file + EXE->e_lfanew);
	modtable = load_importnametable((char*)NE + NE->ne_imptab, NE->ne_cmod);
	segment *segmenttable = (segment*)((char*)NE + NE->ne_segtab);
	*cs = (UINT16)(NE->ne_csip >> 16);
	*ip = (UINT16)(NE->ne_csip & 0xFFFF);
	*ss = (UINT16)(NE->ne_sssp >> 16);
	*sp = (UINT16)(NE->ne_sssp & 0xFFFF);
	*sp = 0xFFFF;//tekitou
	*ss = 0x3000;
	DWORD seg = 0x10000;
	DWORD func_jmp = 0x70000;
	WORD func = 0;
	for (int i = 0; i < NE->ne_cseg; i++)
	{
		int offset = segmenttable[i].offset << NE->ne_align;
		memcpy(mem + seg, file + offset, segmenttable[i].length);
		WORD tablelen = *(WORD*)(file + offset + segmenttable[i].length);
		reloctable *table = (reloctable*)(file + offset + segmenttable[i].length + sizeof(WORD));
		//reloc
		for (int j = 0; j < tablelen; j++)
		{
			relocflag flag = (relocflag)(table[j].flag & TARGET_MASK);
			DWORD addr = 0;
			if (table[j].flag & ADDITIVE)
			{
				NOTIMPL("ADDITIVE\n");
			}
			switch (flag)
			{
			case IMPORTORDINAL:
				addr = 0xA000 + func;
				mem[addr + 0] = 0xCD;
				mem[addr + 1] = WIN16_CALL;
				*(WORD*)(mem + addr + 2) = table[j].importordinal.module - 1;
				*(WORD*)(mem + addr + 4) = table[j].importordinal.ordinal;
				func += 7;
				if (!strcmp(modtable[table[j].importordinal.module - 1], "KERNEL"))
				{
					//とりあえず
#define __AHSHIFT 3
#define __AHINCR 8
#define __AHSHIFTORD 113
#define __AHINCRORD 114
					if (table[j].importordinal.ordinal == __AHSHIFTORD)
					{
						addr = __AHSHIFT;
					}
					if (table[j].importordinal.ordinal == __AHINCRORD)
					{
						addr = __AHINCR;
					}
				}
				dprintf("IMPORTORDINAL module=%s,ordinal=%d\n", modtable[table[j].importordinal.module - 1], table[j].importordinal.ordinal);
				break;
			case INTERNALREF:
				addr = (table[j].internalref.segnum) * 0x1000 + table[j].internalref.offset;
				dprintf("INTERNALREF segnum=%d,offset=0x%X,addr=%04X:%04X\n", table[j].internalref.segnum - 1, table[j].internalref.offset, (table[j].internalref.segnum) * 0x1000 , table[j].internalref.offset);
				break;
			case IMPORTNAME:
				NOTIMPL("relocflag:IMPORTNAME\n");
				break;
			case OSFIXUP:
				NOTIMPL("relocflag:OSFIXUP\n");
				break;
			default:
				NOTIMPL("relocflag:default\n");
				break;
			}
			switch (table[j].type & SOURCE_MASK)
			{
			case LOBYTE:
				NOTIMPL("reloctype:LOBYTE\n");
				break;
			case SEGMENT:
				*(WORD*)(mem + seg + table[j].offset) = addr >> 16;
				break;
			case FAR_ADDR:
				*(DWORD*)(mem + seg + table[j].offset) = addr;
				break;
			case OFFSET:
				*(WORD*)(mem + seg + table[j].offset) = addr;
				break;
			default:
				NOTIMPL("reloctype:default\n");
				break;
			}
		}
		seg += 0x10000;
		dprintf("load segment %X\n", i);
	}
	*cs = 0x1000;
	/*
	for (int i = 0; i < NE->ne_cmod; i++)
	{
		free(modtable[i]);
	}
	free(modtable);*/
}
bool loadne(char *argv[])
{
	
	const char* program = argv[1];
	FILE *fp = fopen(program, "rb");
	if (!fp)
	{
		fprintf(stderr, "can't open file. %s\n", program);
		return false;
	}
	fseek( fp, 0, SEEK_END );
	long size = ftell( fp );
	char *file = (char*)malloc(size);
	fseek(fp, 0, SEEK_SET);
	fread(file, 1, size, fp);
	fclose(fp);
	PIMAGE_DOS_HEADER EXE = (PIMAGE_DOS_HEADER)file;
	if (EXE->e_magic != IMAGE_DOS_SIGNATURE)
	{
		fprintf(stderr, "this is not EXE file. %s\n", program);
		return false;
	}
	//TODO:check
	PIMAGE_OS2_HEADER NE = (PIMAGE_OS2_HEADER)(file + EXE->e_lfanew);
	if (NE->ne_magic != IMAGE_OS2_SIGNATURE)
	{
		fprintf(stderr, "this is not NE file. %s\n", program);
		return false;
	}
	char **modtable = load_importnametable((const char*)NE + NE->ne_imptab, NE->ne_cmod);
	segment *segmenttable = (segment*)((const char*)NE + NE->ne_segtab);
	for (int i = 0; i < NE->ne_cmod; i++)
	{
		free(modtable[i]);
	}
	free(modtable);
	free(file);
	return true;
}
char** load_importnametable(const char *file, int length)
{
	dprintf("IMPORT NAMETABLE\n");
	file++;
	char **modtable = (char**)malloc(length * sizeof(char*));
	for (int i = 0; i < length; i++)
	{
		BYTE strlen = *file++;
		modtable[i] = (char*)malloc(strlen + 1);
		for (int j = 0; j < strlen; j++)
		{
			modtable[i][j] = *file++;
		}
		modtable[i][strlen] = '\0';
		dprintf("%s\n", modtable[i]);
	}

	return modtable;
}