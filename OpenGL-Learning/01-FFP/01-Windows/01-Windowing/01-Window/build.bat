
REM Compiler Command
cl.exe /c /EHsc Window.c

REM Linker
link.exe Window.obj User32.lib GDI32.lib /SUBSYSTEM:WINDOWS

REM Executable file
Window.exe