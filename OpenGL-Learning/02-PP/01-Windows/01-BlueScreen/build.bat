cls

REM cl.exe /c /EHsc /I "D:\\Programming\\RTR\\OpenGL\\GLEW\\glew-2.1.0-win32\\glew-2.1.0\\include"  OGL.cpp

cl.exe /c /EHsc  /I C:\glew\include OGL.c

rc.exe OGL.rc

REM link.exe OGL.obj OGL.res Msimg32.lib User32.lib GDI32.lib  /LIBPATH:"D:\\Programming\\RTR\\OpenGL\\GLEW\\glew-2.1.0-win32\\glew-2.1.0\\lib\\Release\\x64" /SUBSYSTEM:WINDOWS
link.exe OGL.obj OGL.res User32.lib GDI32.lib /LIBPATH:C:\glew\lib\Release\x64 /SUBSYSTEM:WINDOWS

OGL.exe
