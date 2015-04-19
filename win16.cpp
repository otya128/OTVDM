#include "win16.h"
#include "kernel.h"
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
void win16_call_module()
{
	WORD module = *(WORD*)(mem + m_pc);
	WORD ordinal = *(WORD*)(mem + m_pc + 2);
	//NOTIMPL("undefined %s function:%d\n", modtable[module], ordinal);
	if (!strcmp(modtable[module], "KERNEL"))
	{
		kernel_table[ordinal]();
		return;
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
}
void _GetModuleFileName16()
{
	WORD nSize = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4) & 0xffff))));
	DWORD lpFileName = *(DWORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 6) & 0xffff))));
	HINSTANCE16 hInstance = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 10) & 0xffff))));
	pascal_result_int16(GetModuleFileName16(hInstance, (LPSTR)FARPTRToPTR32(lpFileName), nSize));
}
void _InitTask16()
{
	REG16(AX) = 1;
}
#include <map>
#include <string>
HANDLE handle16_mapcnt = 0;
HANDLE16 handle_mapcnt = 0;
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
HANDLE16 HANDLEToHANDLE16(HANDLE handle)
{
	if (!handle) return NULL;
	if (handlemap.find(handle) == handlemap.end())
	{
		handle_mapcnt++;
		handle16map[handle_mapcnt] = handle;
		return handlemap[handle] = handle_mapcnt;
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
	MessageBoxA((HWND)HANDLE16ToHANDLE(hWnd), (LPCSTR)FARPTRToPTR32(lpText), (LPCSTR)FARPTRToPTR32(lpCaption), uType);
	REG16(AX) = 1;
}
//5
void _InitApp16()
{
	HINSTANCE16 hInstance = *(WORD*)(mem + ((m_base[SS] + ((m_regs.w[SP] + 4) & 0xffff))));
	REG16(AX) = TRUE;
}
//6
void _PostQuitMessage16()
{
	WORD nExitCode = *get_int16_arg(0);
	PostQuitMessage(nExitCode);
	REG16(AX) = 1;
}
#define AMASK  m_amask
// offsets and addresses are 32-bit (for now...)
typedef UINT32	offs_t;
void write_word(offs_t byteaddress, UINT16 data);
#define write_word_unaligned write_word
extern UINT16  m_limit[4];
extern UINT8 m_rights[4];
char inging = false;
void cpu_exexute_call_wrap();
LRESULT CALLBACK Win16WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	HWND16 hwnd16 = HANDLEToHANDLE16((HANDLE)hwnd);
	if (hwnd16)
	{
		dprintf("wpp:%d,%X\n", inging, msg, msg, wp, lp);
		char name[256];
		//dprintf("%X\t", msg, msg, wp, lp);
		GetClassNameA(hwnd, name, sizeof(name));
		std::map<std::string, DWORD>::iterator itr;
		if ((itr = wprocmap.find(name)) != wprocmap.end())
		{/*
			if (inging == 2)
			{
				inging = 3;
				return DefWindowProc(hwnd, msg, wp, lp);
			}*///if (inging) inging = 2;
			WORD cs, ip;
			ip = m_pc - m_base[SREGS::CS];
			cs = m_sregs[CS];
			UINT16 selector = itr->second >> 16, offset = (UINT16)itr->second;
			m_sregs[CS] = selector;
			m_base[CS] = selector << 4;
			m_rights[CS] = 0x9a;
			m_limit[CS] = 0xffff;
			UINT16 stk = m_regs.w[SP];
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), hwnd16);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), msg);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), wp);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), lp >> 16);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), lp & 0xFFFF);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), cs);
			m_regs.w[SP] -= 2;
			WriteWord(((m_base[SS] + m_regs.w[SP]) & AMASK), ip);
			m_pc = (m_base[CS] + offset)&AMASK;
			//dprintf("hwnd:%X,msg:%X,wp:%X,lp:%X,cs:%X,ip:%X\n",hwnd16,msg,wp,lp,cs,ip);
				//CreateWindowの中でも呼ばれるので強引に
				//メッセージを溜めるようにした方がいいかもしれない
			while (m_regs.w[SP] < stk)
			{
				//dprintf("%X\t", m_pc, msg, wp, lp, cs, ip);
				cpu_exexute_call_wrap();
			}
			//dprintf("%X\n", m_pc, msg, wp, lp, cs, ip);
			return REG16(AX) | REG16(DX) << 16;
		}
	}
	dprintf("wpp:%d,%X\n", inging, msg, msg, wp, lp);
	return DefWindowProc(hwnd, msg, wp, lp);
}
//41
#define CW_USEDEFAULT16   ((short)0x8000)
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
	inging = true;
	HWND hWnd = CreateWindowExA(0, className, windowName, style, x == CW_USEDEFAULT16 ? CW_USEDEFAULT : x, y == CW_USEDEFAULT16 ? CW_USEDEFAULT : y, width, height, (HWND)HANDLE16ToHANDLE(parent), (HMENU)HANDLE16ToHANDLE(menu), (HINSTANCE)HANDLE16ToHANDLE(instance), (LPVOID)data);
	inging = false;
	REG16(AX) = HANDLEToHANDLE16(hWnd);
}
void _RegisterClass16()
{
	WNDCLASS16 *wc = (WNDCLASS16*)FARPTRToPTR32(*get_int32_arg(0));
	WNDCLASSEXA wca;
	wca.cbSize = sizeof(wca);
	wca.style = wc->style;
	wca.lpfnWndProc = Win16WndProc;
	wca.hInstance = (HINSTANCE)HANDLE16ToHANDLE(wc->hInstance);
	wca.cbClsExtra = wc->cbClsExtra;
	wca.cbWndExtra = wc->cbWndExtra;
	wca.hIcon = (HICON)HANDLE16ToHANDLE(wc->hIcon);
	wca.hCursor = (HCURSOR)HANDLE16ToHANDLE(wc->hCursor);
	wca.hbrBackground = (HBRUSH)HANDLE16ToHANDLE(wc->hbrBackground);
	wca.lpszMenuName = (LPCSTR)FARPTRToPTR32(wc->lpszMenuName);
	wca.lpszClassName = (LPCSTR)FARPTRToPTR32(wc->lpszClassName);
	wca.hIconSm = NULL;
	wprocmap[wca.lpszClassName] = wc->lpfnWndProc;
	ATOM atom = RegisterClassExA(&wca);
	REG16(AX) = atom;
}
//107
void _DefWindowProc16()
{
	int argc = 0;
	LPARAM16 lp = get_int32_argex(&argc);
	WPARAM16 wp = get_int16_argex(&argc);
	UINT16 msg = get_int16_argex(&argc);
	HWND16 hwnd = get_int16_argex(&argc);
	HWND hwnd32 = (HWND)HANDLE16ToHANDLE(hwnd);
	//TODO:result
	dprintf("hwnd:%X,msg:%X,wp:%X,lp:%X\n", hwnd, msg, wp, lp);
	inging = 2;
	REG16(AX) = DefWindowProcA(hwnd32, msg, wp, lp);
	inging = 0;
}
void _GetMessage16()
{
	int argc = 0;
	UINT16 last = get_int16_argex(&argc);
	UINT16 first = get_int16_argex(&argc);
	HWND16 hWnd = get_int16_argex(&argc);
	DWORD lpMsg = get_int32_argex(&argc);
	HWND hWnd32 = (HWND)HANDLE16ToHANDLE(hWnd);
	MSG16 *msg = (MSG16*)FARPTRToPTR32(lpMsg);
	MSG msg32;
/*	msg32.hwnd = (HWND)HANDLE16ToHANDLE(msg->hwnd);
	msg32.message = msg->message;
	msg32.wParam = msg->wParam;
	msg32.lParam = msg->lParam;
	msg32.time = msg->time;
	msg32.pt.x = msg->pt.x;
	msg32.pt.y = msg->pt.y;*/
	REG16(AX) = GetMessageA(&msg32, hWnd32, first, last);
	msg->hwnd = HANDLEToHANDLE16(msg32.hwnd);
	msg->message = msg32.message;
	msg->wParam = msg32.wParam;
	msg->lParam = msg32.lParam;
	msg->time = msg32.time;
	msg->pt.x = msg32.pt.x;
	msg->pt.y = msg32.pt.y;
}
//114
//返り血不明
void _DispatchMessage16()
{
	MSG16* msg = (MSG16*)FARPTRToPTR32(*get_int32_arg(0));
	MSG msg32;
	msg32.hwnd = (HWND)HANDLE16ToHANDLE(msg->hwnd);
	msg32.message = msg->message;
	msg32.wParam = msg->wParam;
	msg32.lParam = msg->lParam;
	msg32.time = msg->time;
	msg32.pt.x = msg->pt.x;
	msg32.pt.y = msg->pt.y;
	DispatchMessageA(&msg32);
}
void _GetStockObject16()
{
	REG16(AX) = HANDLEToHANDLE16(GetStockObject(*get_int16_arg(0)));
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
	*cs = (UINT16)(NE->ne_csip >> 16)* 0x1000;
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
				addr = (table[j].internalref.segnum) * 0x10000 + table[j].internalref.offset;
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
				*(WORD*)(mem + seg + table[j].offset) = (addr & 0xFFFF0000) >> 4;
				break;
			case FAR_ADDR:
				*(DWORD*)(mem + seg + table[j].offset) = addr;
				//*(WORD*)(mem + seg + table[j].offset) = addr & 0xFFFF;
				//*(WORD*)(mem + seg + table[j].offset + 2) = (addr & 0xFFFF0000)>>4;
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
	//toriaezu
	//*cs = 0x1000;
	*di = 0x0000;
	*ds = 0x2000;
	*di = 0x0000;
	*ds = NE->ne_autodata * 0x1000;
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