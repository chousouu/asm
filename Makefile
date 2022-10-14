CC=g++
CFLAGS= -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual
all: asm

asm: asm.cpp 
	   	   $(CC) asm.cpp -o asm.exe $(CFLAGS)
		   
cpu: stack.cpp cpu.cpp
	   	   $(CC) stack.cpp cpu.cpp -o cpu.exe $(CFLAGS)


clean:
	   del *.o *.exe