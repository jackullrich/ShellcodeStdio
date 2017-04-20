#include "ScStdio.h"

namespace ScStdio {
	/*
		Suggested VS Compilation Switches:
		C/C++ -> Optimization -> /O1, /Ob2, /Oi, /Os, /Oy-, /GL
		C/C++ -> Code Generation -> /MT, /GS-, /Gy
		Linker -> General -> /INCREMENTAL:NO
	*/

	__declspec(naked) void MalCodeBegin() { __asm { jmp MalCode } };

#define htons(A) ((((WORD)(A) & 0xff00) >> 8) | (((WORD)(A) & 0x00ff) << 8))

	__forceinline PEB *get_peb() {
		PEB *p;
		__asm {
			mov eax, fs:[30h]
			mov p, eax
		}
		return p;
	}

#define ROR_SHIFT 13

	constexpr DWORD ct_ror(DWORD n) {
		return (n >> ROR_SHIFT) | (n << (sizeof(DWORD) * CHAR_BIT - ROR_SHIFT));
	}

	constexpr char ct_upper(const char c) {
		return (c >= 'a') ? (c - ('a' - 'A')) : c;
	}

	constexpr DWORD ct_hash(const char *str, DWORD sum = 0) {
		return *str ? ct_hash(str + 1, ct_ror(sum) + ct_upper(*str)) : sum;
	}

	DWORD rt_hash(const char *str) {
		DWORD h = 0;
		while (*str) {
			h = (h >> ROR_SHIFT) | (h << (sizeof(DWORD) * CHAR_BIT - ROR_SHIFT));
			h += *str >= 'a' ? *str - ('a' - 'A') : *str;
			str++;
		}
		return h;
	}

	LDR_DATA_TABLE_ENTRY *getDataTableEntry(const LIST_ENTRY *ptr) {
		int list_entry_offset = offsetof(LDR_DATA_TABLE_ENTRY, InMemoryOrderLinks);
		return (LDR_DATA_TABLE_ENTRY *)((BYTE *)ptr - list_entry_offset);
	}

	PVOID getProcAddrByHash(DWORD hash) {
		PEB *peb = get_peb();
		LIST_ENTRY *first = peb->Ldr->InMemoryOrderModuleList.Flink;
		LIST_ENTRY *ptr = first;
		do {
			LDR_DATA_TABLE_ENTRY *dte = getDataTableEntry(ptr);
			ptr = ptr->Flink;

			BYTE *baseAddress = (BYTE *)dte->DllBase;
			if (!baseAddress)
				continue;
			IMAGE_DOS_HEADER *dosHeader = (IMAGE_DOS_HEADER *)baseAddress;
			IMAGE_NT_HEADERS *ntHeaders = (IMAGE_NT_HEADERS *)(baseAddress + dosHeader->e_lfanew);
			DWORD iedRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
			if (!iedRVA)
				continue;
			IMAGE_EXPORT_DIRECTORY *ied = (IMAGE_EXPORT_DIRECTORY *)(baseAddress + iedRVA);
			char *moduleName = (char *)(baseAddress + ied->Name);
			DWORD moduleHash = rt_hash(moduleName);
			DWORD *nameRVAs = (DWORD *)(baseAddress + ied->AddressOfNames);
			for (DWORD i = 0; i < ied->NumberOfNames; ++i) {
				char *functionName = (char *)(baseAddress + nameRVAs[i]);
				if (hash == moduleHash + rt_hash(functionName)) {
					WORD ordinal = ((WORD *)(baseAddress + ied->AddressOfNameOrdinals))[i];
					DWORD functionRVA = ((DWORD *)(baseAddress + ied->AddressOfFunctions))[ordinal];
					return baseAddress + functionRVA;
				}
			}
		} while (ptr != first);

		return NULL;
	}

#define DEFINE_FUNC_PTR(module, function) \
	constexpr DWORD hash_##function = ct_hash(module) + ct_hash(#function); \
	typedef decltype(function) type_##function; \
	type_##function *##function = (type_##function *)getProcAddrByHash(hash_##function)

#define DEFINE_FWD_FUNC_PTR(module, real_func, function) \
	constexpr DWORD hash_##function = ct_hash(module) + ct_hash(real_func); \
	typedef decltype(function) type_##function; \
	type_##function *##function = (type_##function *)getProcAddrByHash(hash_##function)

	VOID __stdcall MalCode() {

		CHAR strUser32[] = { 'u','s','e','r','3','2','.','d','l','l',0 };
		CHAR strMboxTitle[] = { 'S','h','e','l','l','S','t','d','i','o', 0 };
		CHAR strMboxMsg[] = { 'H','e','l','l','o',' ', 'W','o','r','l','d','!',0 };

		DEFINE_FUNC_PTR("kernel32.dll", LoadLibraryA);
		LoadLibraryA(strUser32);

		DEFINE_FUNC_PTR("user32.dll", MessageBoxA);
		MessageBoxA(NULL, strMboxMsg, strMboxTitle, MB_OK);
	}

	__declspec(naked) void MalCodeEnd() { };

	BOOL WriteShellcodeToDisk()
	{
		DWORD dwWritten;
		HANDLE FileHandle = CreateFileW(L"shellcode.bin", GENERIC_ALL, NULL, NULL, CREATE_ALWAYS, NULL, NULL);

		if (!FileHandle)
			return false;

		if (WriteFile(FileHandle, &MalCodeBegin, ((DWORD)&MalCodeEnd - (DWORD)&MalCodeBegin), &dwWritten, NULL))
		{
			CloseHandle(FileHandle);
			return true;
		}

		CloseHandle(FileHandle);
		return false;
	}
}