### ShellcodeStdio
A mini-framework for easily writing debuggable, compiler optimized, position independent, x86 shellcode for windows platforms.

For basic usage see main.cpp

### Suggested compiler options
```
C/C++ -> Optimization -> /O1, /Ob2, /Oi, /Os, /Oy-, /GL
C/C++ -> Code Generation -> /MT, /GS-, /Gy
Linker -> General -> /INCREMENTAL:NO
```
### How do I use this?
When working with this framework, and coding shellcode in general, you'll have to keep a few things in mind:

1. You must declare all your variables on the stack.
2. Strings are no exception. This is how to create a char*, wchar_t* on the stack. Don't forget to null-terminate.
```C++
unsigned char str[] = { 'E', 'x', 'a', 'p', 'l', 'e', 0 }; 
unsigned char w_str[] = { 'E', 0, 'x', 0, 'a', 0, 'm', 0, 'p', 0, 'l', 0, 'e', 0, 0 };
```
3. If you want to use an external function (i.e Windows API) you must first ensure the module containing the function is loaded into the current process. 
4. The second part of calling an external function is calling the DEFINE_FUNC_PTR or DEFINE_FWD_FUNC_PTR macro with the name of the module and the typedef of the function you want to call.

### Hello World Example
```C++
CHAR strUser32[] = { 'u','s','e','r','3','2','.','d','l','l',0 };
CHAR strMboxTitle[] = { 'S','h','e','l','l','S','t','d','i','o', 0 };
CHAR strMboxMsg[] = { 'H','e','l','l','o',' ', 'W','o','r','l','d','!',0 };

DEFINE_FUNC_PTR("kernel32.dll", LoadLibraryA);
LoadLibraryA(strUser32);

DEFINE_FUNC_PTR("user32.dll", MessageBoxA);
MessageBoxA(NULL, strMboxMsg, strMboxTitle, MB_OK);
```

### Misc
* If you wish to add a function you must keep it between the MalCodeBegin and MalCodeEnd markers. 
* Don't forget to call the ScStdio::MalCode() at some point, otherwise when compiling in release with the suggested switches, the code will optimized away.
