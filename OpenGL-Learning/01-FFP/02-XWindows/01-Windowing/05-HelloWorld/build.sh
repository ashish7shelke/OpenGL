#GNU Compiler Collection 

#c1 Compiler
#-c Compile
#-o Output
#-I Include 

gcc -c -o XWindow.o -I /usr/include/ XWindow.c 

#ld linker
#-lX11 l means link & X11 is library file name ---> libX11.so
gcc -o XWindow -L /usr/lib/x86_64-linux-gnu XWindow.o -lX11
