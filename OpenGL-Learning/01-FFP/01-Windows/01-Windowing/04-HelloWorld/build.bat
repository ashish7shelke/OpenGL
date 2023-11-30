cl.exe /c /EHsc Window.c

link.exe Window.obj  Msimg32.lib	 User32.lib GDI32.lib /SUBSYSTEM:WINDOWS

Window.exe