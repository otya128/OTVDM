#include "win16.h"
#include "kernel.h"
#include "user.h"
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
//引数月RETは引数BYTEスタックをpopする
char **modtable;
typedef void(*modulehandler)();
void KERNEL_call(WORD ordinal);
modulehandler kernel_table[2048];
modulehandler user_table[2048];
modulehandler gdi_table[2048];
int win16_init();
int _win16_init = win16_init();
typedef struct
{

} INSTANCE16;
UINT16 i80286_far_return_wrap(int iret, int bytes);
void win16_call_module()
{
	WORD module = *(WORD*)(mem + m_pc);
	WORD ordinal = *(WORD*)(mem + m_pc + 2);
	NOTIMPL("call %s function:%d\n", modtable[module], ordinal);
	//NOTIMPL("undefined %s function:%d\n", modtable[module], ordinal);
	if (!strcmp(modtable[module], "KERNEL"))
	{
		if (kernel_table[ordinal])
		{
			kernel_table[ordinal]();
			return;
		}
	}
	if (!strcmp(modtable[module], "USER"))
	{
		if (user_table[ordinal])
		{
			user_table[ordinal]();
			return;
		}
	}
	if (!strcmp(modtable[module], "GDI"))
	{
		if (gdi_table[ordinal])
		{
			gdi_table[ordinal]();
			return;
		}
	}
	NOTIMPL("undefined %s function:%d\n", modtable[module], ordinal);
	i80286_far_return_wrap(0, 0);
}
//呼び出し規約pascalでの返り値
//16bit以内の場合AX//8bitの場合は知らない
void pascal_result_int16(WORD result)
{
	REG16(AX) = result;
}
//32bitの場合DXが上位(なはず)
void pascal_result_int32(DWORD result)
{
	REG16(AX) = result & 0xFFFF;
	REG16(DX) = result >> 16;
}
//pascalの場合
//func(a,b,c)
//の場合順番に
//PUSH a,PUSH b,PUSH cされる
//なのでnは新しい順
VOID *get_arg(int n)
{
	return (WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff))));
}
WORD *get_int16_arg(int n)
{
	return (WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff))));
}
WORD get_int16_argnp(int n)
{
	return *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff))));
}
WORD get_int16_argex(int *n)
{
	WORD* _ = (WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + *n) & 0xffff))));
	*n += sizeof(WORD);
	return *_;
}
DWORD *get_int32_arg(int n)
{
	return (DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff))));
}
DWORD get_int32_argnp(int n)
{
	return *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff))));
}
DWORD get_int32_argex(int *n)
{
	DWORD* _ = (DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + *n) & 0xffff))));
	*n += sizeof(DWORD);
	return *_;
}
char *get_string(int n)
{
	return (char*)FARPTRToPTR32((DWORD)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + n) & 0xffff)))));
}
char *get_stringex(int *n)
{
	char* _ = (char*)FARPTRToPTR32(*(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4 + *n) & 0xffff)))));
	*n += sizeof(DWORD);
	return _;
}
void _WaitEvent16()
{
	pascal_result_int16(WaitEvent16(get_int16_argnp(0)));
	i80286_far_return_wrap(0, 2);
}
void _GetModuleFileName16()
{
	WORD nSize = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4) & 0xffff))));
	DWORD lpFileName = *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 6) & 0xffff))));
	HINSTANCE16 hInstance = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 10) & 0xffff))));
	pascal_result_int16(GetModuleFileName16(hInstance, (LPSTR)FARPTRToPTR32(lpFileName), nSize));
	i80286_far_return_wrap(0, 8);
}
void _InitTask16()
{
	InitTask16();
	i80286_far_return_wrap(0, 0);
}
#include <map>
#include <string>
//ハンドルの総数
WORD handle16_mapcnt = 0;
std::map<std::string, DWORD> wprocmap;
std::map<HANDLE, HANDLE16> handlemap;
std::map<HANDLE16, HANDLE> handle16map;
HANDLE HANDLE16ToHANDLE(HANDLE16 handle16)
{
	if (!handle16) return NULL;
	if (handle16map.find(handle16) == handle16map.end())
		return NULL;
	else
		return handle16map[handle16];
}
HANDLE16 freeHANDLE16 = 1;
HANDLE16Data HANDLE16array[65536];
HANDLE16Data *gethandledata(HANDLE16 handle)
{
	return &HANDLE16array[(WORD)handle];
}
//ハンドルを割り当てる
//詳しくは知らないけどKRNL386.EXE,USER.EXE,GDI.EXEごとにハンドルが分かれそうな感じがする
HANDLE16 AllocHANDLE16()
{
	HANDLE16 ret = freeHANDLE16;
	freeHANDLE16 = HANDLE16array[(WORD)ret].next;
	handle16_mapcnt++;
	return ret;
}
void FreeHANDLE16(HANDLE16 handle)
{
	HANDLE16array[handle].next = freeHANDLE16;
	freeHANDLE16 = handle;
	handle16_mapcnt--;
}
//ハンドルを開放する
HANDLE16 HANDLEToHANDLE16(HANDLE handle)
{
	if (!handle) return NULL;
	if (handlemap.find(handle) == handlemap.end())
	{
		HANDLE16 handle16 = AllocHANDLE16();
		handle16map[handle16] = handle;
		return handlemap[handle] = handle16;
	}
	else
		return handlemap[handle];
}
void *FARPTRToPTR32(DWORD farptr)
{
	WORD segment = farptr >> 16;
	WORD ptr = farptr;
	if (!(mem + segment * 16 + ptr))return NULL;
	return mem + segment * 16 + ptr;
}

void _MessageBox16()
{
	HWND16 hWnd = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 14) & 0xffff))));
	DWORD lpText = *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 10) & 0xffff))));
	DWORD lpCaption = *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 6) & 0xffff))));
	WORD uType = *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4) & 0xffff))));
	pascal_result_int16(MessageBox16(hWnd, (LPCSTR)FARPTRToPTR32(lpText), (LPCSTR)FARPTRToPTR32(lpCaption), uType));
	i80286_far_return_wrap(0, 12);
}
//5
void _InitApp16()
{
	HINSTANCE16 hInstance = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4) & 0xffff))));
	pascal_result_int16(InitApp16(hInstance));
	i80286_far_return_wrap(0, 2);
}
//6
void _PostQuitMessage16()
{
	WORD nExitCode = *get_int16_arg(0);
	i80286_far_return_wrap(0, 2);
	PostQuitMessage16(nExitCode);
}
DWORD global_stack = 0xE0000;
#define GPUSH(val)               { WriteByte(((global_stack) & AMASK), val); global_stack += 1;}
DWORD i86_galloca_ptr(void *ptr, WORD size)
{
	char *wp = (char*)ptr;
	DWORD p = global_stack;
	while (--size)
	{
		GPUSH(*wp++);
	}
	return (p << 12) & 0xF0000000 | (p & 0xFFFF);
}
void i86_gfree_ptr(WORD size)
{
	global_stack += size;
}

//41
void _CreateWindow16()
{
	int argc = 0;
	LPVOID16 data = get_int32_argex(&argc);
	HINSTANCE16 instance = get_int16_argex(&argc);
	HMENU16 menu = get_int16_argex(&argc);
	HWND16 parent = get_int16_argex(&argc);
	INT16 height = get_int16_argex(&argc);
	INT16 width = get_int16_argex(&argc);
	INT16 y = get_int16_argex(&argc);
	INT16 x = get_int16_argex(&argc);
	DWORD style = get_int32_argex(&argc);
	char *windowName = get_stringex(&argc);
	char *className = get_stringex(&argc);
	pascal_result_int16(CreateWindow16(className, windowName,
		style, x, y, width, height, parent, menu,
		instance, data));
	i80286_far_return_wrap(0, argc);
}
void _RegisterClass16()
{
	WNDCLASS16 *wc = (WNDCLASS16*)FARPTRToPTR32(*get_int32_arg(0));
	pascal_result_int16(RegisterClass16(wc));
	i80286_far_return_wrap(0, 4);
}
//107
void _DefWindowProc16()
{
	int argc = 0;
	LPARAM lp = get_int32_argex(&argc);
	WPARAM wp = get_int16_argex(&argc);
	UINT16 msg = get_int16_argex(&argc);
	HWND16 hwnd = get_int16_argex(&argc);
	pascal_result_int32(DefWindowProc16(hwnd, msg, wp, lp));
	i80286_far_return_wrap(0, argc);
}
void _GetMessage16()
{
	int argc = 0;
	UINT16 last = get_int16_argex(&argc);
	UINT16 first = get_int16_argex(&argc);
	HWND16 hWnd = get_int16_argex(&argc);
	DWORD lpMsg = get_int32_argex(&argc);
	pascal_result_int16(GetMessage16((MSG16*)FARPTRToPTR32(lpMsg), hWnd, first, last));
	i80286_far_return_wrap(0, argc);
}
//114
void _DispatchMessage16()
{
	MSG16* msg = (MSG16*)FARPTRToPTR32(*get_int32_arg(0));
	//pascal_result_int32
		(DispatchMessage16(msg));
	i80286_far_return_wrap(0, 4);
}
void _GetStockObject16()
{
	REG16(AX) = HANDLEToHANDLE16(GetStockObject(*get_int16_arg(0)));
	i80286_far_return_wrap(0, 2);
}
//pascalは順番にスタックに積む
int win16_init()
{
	kernel_table[30] = _WaitEvent16;
	kernel_table[49] = _GetModuleFileName16;
	kernel_table[91] = _InitTask16;
	user_table[1] = _MessageBox16;
	user_table[5] = _InitApp16;
	user_table[6] = _PostQuitMessage16;
	user_table[41] = _CreateWindow16;
	user_table[57] = _RegisterClass16;
	user_table[107] = _DefWindowProc16;
	user_table[108] = _GetMessage16;
	user_table[114] = _DispatchMessage16;
	gdi_table[87] = _GetStockObject16;
	//init HANDLE16array
	for (int i = 1; i < 65536; i++)
	{
		HANDLE16array[i].next = i + 1;
	}
	return 0;
}
//segment* load_segmentable(const char *file, int length);
//cs:ip,ss:sp
//moduleの関数呼び出し
//とりあえず1000:あたりに置いてみる
//module tableをつくる
//面倒だからとりあえず重複ありで考える
void dos_loadne(UINT8 *file, UINT16 *cs, UINT16 *ss, UINT16 *ip, UINT16 *sp, UINT16 *di, UINT16 *ds, UINT8 *mem)
{
	PIMAGE_DOS_HEADER EXE = (PIMAGE_DOS_HEADER)file;
	PIMAGE_OS2_HEADER NE = (PIMAGE_OS2_HEADER)(file + EXE->e_lfanew);
	modtable = load_importnametable((char*)NE + NE->ne_imptab, NE->ne_cmod);
	segment *segmenttable = (segment*)((char*)NE + NE->ne_segtab);
	*cs = (UINT16)(NE->ne_csip >> 16)* 0x1000 + 1;
	*ip = (UINT16)(NE->ne_csip & 0xFFFF);
	*ss = (UINT16)(NE->ne_sssp >> 16);
	*sp = (UINT16)(NE->ne_sssp & 0xFFFF);
	*sp = 0xFFFF;//tekitou
	*ss = 0x3000;
	//最初の16?byteは予約
	//DWORD startseg = 0x10010;
	//DWORD seg =  startseg;
	DWORD func_jmp = 0x70000;
	WORD func = 0;
	HGLOBAL16 taskmem = GlobalAlloc16(0, sizeof(TASK16) + sizeof(segmenttable_t) * NE->ne_cseg);
	char *taskptr = (char*)FARPTRToPTR32(GlobalLock16(taskmem));
	TASK16 *task = (TASK16*)taskptr;
	segmenttable_t *hsegtable = (segmenttable_t*)(task + sizeof(TASK16));
	for (int i = 0; i < NE->ne_cseg; i++)
	{
		hsegtable[i].offset = segmenttable[i].offset;
		hsegtable[i].length = segmenttable[i].length;
		hsegtable[i].flag = segmenttable[i].flag;
		hsegtable[i].minsize = segmenttable[i].minsize;
	}
	for (int i = 0; i < NE->ne_cseg; i++)
	{
		HGLOBAL16 hMem = GlobalAlloc16(0, segmenttable[i].length);
		hsegtable[i].hMem = hMem;
		//gethandledata(hMem)->data = 0x10000000 * (i+1);
		LPVOID16 seg1 = GlobalLock16(hMem);
		BYTE *segmem = (BYTE*)FARPTRToPTR32(seg1);
		int offset = segmenttable[i].offset << NE->ne_align;
		memcpy(segmem, file + offset, segmenttable[i].length);
	}
	for (int i = 0; i < NE->ne_cseg; i++)
	{
		HGLOBAL16 hMem = hsegtable[i].hMem;
		LPVOID16 seg1 = GlobalLock16(hMem);
		BYTE *segmem = (BYTE*)FARPTRToPTR32(seg1);
		int offset = segmenttable[i].offset << NE->ne_align;
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
				addr = GlobalLock16(hsegtable[table[j].internalref.segnum - 1].hMem) + table[j].internalref.offset;
				//addr |= 0x00010000;
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
				*(WORD*)(segmem + table[j].offset) = (addr & 0xFFFF0000) >> 16;
				break;
			case FAR_ADDR:
				*(DWORD*)(segmem + table[j].offset) = addr;
				//*(WORD*)(mem + seg + table[j].offset) = addr & 0xFFFF;
				//*(WORD*)(mem + seg + table[j].offset + 2) = (addr & 0xFFFF0000)>>4;
				break;
			case OFFSET:
				*(WORD*)(segmem + table[j].offset) = addr;
				break;
			default:
				NOTIMPL("reloctype:default\n");
				break;
			}
		}
		//seg += 0x10000;
		dprintf("load segment %X\n", i);
	}
	//toriaezu
	//*cs = 0x1000;
	*di = 0x0000;
	*ds = 0x2000;
	*di = 0x0000;
	*ds = (GlobalLock16(hsegtable[NE->ne_autodata - 1].hMem) >> 16);//NE->ne_autodata * 0x1000 + 1;
	*cs = (GlobalLock16(hsegtable[(NE->ne_csip >> 16) - 1].hMem) >> 16);
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