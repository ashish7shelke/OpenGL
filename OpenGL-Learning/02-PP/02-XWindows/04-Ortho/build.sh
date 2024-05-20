#GNU Compiler Collection 

#c1 Compiler
#-c Compile
#-o Output
#-I Include 

g++ -c -o OGL.o -I /usr/include/ OGL.cpp 

#ld linker
#-lX11 l means link & X11 is library file name ---> libX11.so
g++ -o OGL OGL.o -L /usr/lib/x86_64-linux-gnu -lX11 -lGL -lGLEW
