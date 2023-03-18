D= gcc -c -m32
D2= gcc -m32

controller: controller.o controller-asm.o
	$(D2) controller.o controller-asm.o -o controller
controller.o: controller.c
	$(D) controller.c -o controller.o
controller-asm.o: controller-asm.s
	$(D) controller-asm.s -o controller-asm.o
clean:
	rm -f *.o