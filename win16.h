#ifndef __WIN16__H__
#include <windows.h>
#include <winioctl.h>
#include <tchar.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <conio.h>
#include <math.h>
#include <dos.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <sys/locking.h>
#include <mbctype.h>
#include <direct.h>
#include <errno.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <winnt.h>
//#include "mame/emu/cpu/i86/i286.c"
#ifdef _DEBUG
#define dprintf printf
#else
#define dprintf(...)
#endif
#define WARN printf("WARN:");printf
bool isNE(const char* file);
void dos_loadne(UINT8 *file, UINT16 *cs, UINT16 *ss, UINT16 *ip, UINT16 *sp, UINT16 *di, UINT16 *ds, UINT8 *mem);
bool is_started_from_command_prompt();
bool loadne(char *argv[]);
char** load_importnametable(const char *file, int length);
typedef enum : WORD
{
	DataSegment = 1,//Code
	A = 2,
	Loaded = 4,
	//reserved8
	Movable = 8,
	Pure = 16,
	Shareable = 16,
	Preload = 32,
	ExcuteOnly = 64,
} segmentflag;
typedef struct
{
	WORD offset;
	WORD length;
	segmentflag flag;
	WORD minsize;
} segment;
#define WIN16_CALL 0xF7
void win16_call_module();
void enable_visualstyle(char *path);
#define __WIN16__H__
#endif
