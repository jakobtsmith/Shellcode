shell: shellcode.o functions.o
	gcc -o shell shellcode.o functions.o
shellcode.o: shellcode.c functions.h
	gcc -c shellcode.c
functions.o: functions.c functions.h
	gcc -c functions.c
clean:
	rm *.o shell