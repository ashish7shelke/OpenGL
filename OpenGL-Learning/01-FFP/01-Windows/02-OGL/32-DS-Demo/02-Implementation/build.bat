cls

cl.exe /c /EHsc /I C:\soil\inc OGL.c Scene0_Intro.c Scene1_Vasana.c Scene2_End.c
rc.exe OGL.rc

link.exe OGL.obj Scene0_Intro.obj Scene1_Vasana.obj Scene2_End.obj OGL.res Msimg32.lib User32.lib GDI32.lib /LIBPATH:C:\soil\lib SOIL.lib  /SUBSYSTEM:WINDOWS

OGL.exe