# ShellcodeStdio
A mini-framework for easily writing debuggable, compiler optimized, position independent, x86 shellcode for windows platforms.

For usage see main.cpp

# Suggested Compiler Options
```
C/C++ -> Optimization -> /O1, /Ob2, /Oi, /Os, /Oy-, /GL
C/C++ -> Code Generation -> /MT, /GS-, /Gy
Linker -> General -> /INCREMENTAL:NO
