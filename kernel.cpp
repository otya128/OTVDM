#include <windows.h>
#include "wow32.h"
#include "kernel.h"
#include <vector>
DWORD win16_heap = 0x10000;
DWORD win16_heap_size = 0xE0000 - 0x10000;
typedef struct
{
	DWORD pos;
	DWORD size;
} globalalloc_data;
std::vector<globalalloc_data> globalalloc;
int init_kernel()
{
	globalalloc.push_back({ win16_heap, win16_heap_size });
	return 0;
}
int kernel_unused = init_kernel();
//memo
//GlobalAllocしたあとに
//GlobalLockでアドレス取得
//15
HGLOBAL16 GlobalAlloc16(UINT16 flags, DWORD size)
{
	int allocblock = -1;
	bool just = false;//ちょうど16byte境界にあったか
	DWORD align_pos;
	int diff = 0;
	if ((size >> 4) << 4 != size)
		size = ((size + 16) >> 4) << 4;
	for (unsigned int i = 0; i < globalalloc.size(); i++)
	{
		auto j = globalalloc[i];
		if (j.size >= size)
		{
			if ((j.pos >> 4) << 4 == j.pos)
			{
				//採用
				just = true;
				allocblock = i;
				break;
			}
			align_pos = ((j.pos + 16) >> 4) << 4;
			diff = align_pos - j.pos;
			if (j.size - diff >= size)
			{
				//採用
				allocblock = i;
				break;
			}
		}
	}
	if (allocblock == -1)
		return NULL;//失敗
	globalalloc_data &block = globalalloc[allocblock];
	DWORD allocpos = block.pos + diff;
	if (just && block.size == size)
	{
		//サイズが一致したのでこれは消滅
		globalalloc.erase(globalalloc.begin() + allocblock);
	}
	else
	{
		int block2 = block.pos + size;
		int block2siz = block.size - diff - size;
		if (diff)
		{
			block.size = diff;
			globalalloc.push_back({ block2, block2siz });
		}
		else
		{
			block.pos = block2;
			block.size = block2siz;
		}
	}
	dprintf("GLOBALalloc memory addr=%08X,size=%08X,flags=%X\n", allocpos, size, flags);
	HANDLE16 mem = AllocHANDLE16();
	HANDLE16Data *data = gethandledata(mem);
	data->data = allocpos << 12;
	return mem;
}
//18
LPVOID16 GlobalLock16(HGLOBAL16 hMem)
{
	return gethandledata(hMem)->data;
}
//30
BOOL16 WaitEvent16(HTASK16 hTask)
{
	return 0;
}
HTASK16 CurrentTask = 0;//とりあえず
//36
HTASK16 GetCurrentTask16()
{
	return CurrentTask;
}
//49
INT16 GetModuleFileName16(HINSTANCE16 hModule, LPSTR lpFileName, INT16 nSize)
{
	return 0;
}
//58
INT16 GetProfileString16(LPCSTR lpSect, LPCSTR lpKey, LPCSTR lpDefault, LPSTR lpReturn, INT16 nSize)
{
	NOTIMPL("GetProfileString(\"%s\",\"%s\",\"%s\",0x%X,%d)\n", lpSect, lpKey, lpDefault, lpReturn, nSize);
	if (nSize)
		*lpReturn = '\0';
	return 0;
}
//レジスタ返し
//91
void InitTask16()
{
	HTASK16 task = GetCurrentTask16();
	HANDLE16Data *data = gethandledata(task);
	REG16(AX) = 1;//正常v
	REG16(SI) = 0;//prev instance
}
