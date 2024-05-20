#GNU Compiler Collection 

#c1 Compiler
#-c Compile
#-o Output
#-I Include 

gcc -c -o OGL.o -I /usr/include/ OGL.c

#ld linker
#-lX11 l means link & X11 is library file name ---> libX11.so
gcc -o OGL -L /usr/lib/x86_64-linux-gnu OGL.o -lX11 -lGL -lGLU -lSOIL -lm -lopenal
