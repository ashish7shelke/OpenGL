cls

cl.exe /c /EHsc OGL.c

rc.exe OGL.rc

link.exe OGL.obj OGL.res Msimg32.lib User32.lib GDI32.lib  /SUBSYSTEM:WINDOWS

OGL.exe