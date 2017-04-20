#pragma once

/* Original: ShellcodeStdio -> @ winternl.com (Jack Ullrich)*/
/* Much credit to the reddit user: good_nickname */

#include <Windows.h>
#include <winnt.h>
#include <winternl.h>

namespace ScStdio {
	VOID __stdcall MalCode();
	BOOL WriteShellcodeToDisk();
}