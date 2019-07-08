#pragma once

#include <Windows.h>
#include <winnt.h>
#include <winternl.h>

namespace ScStdio {
	VOID __stdcall MalCode();
	BOOL WriteShellcodeToDisk();
}