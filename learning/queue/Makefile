queue: main.o queue.o

main.o: main.c queue.c 
	gcc -Wall -Werror -ggdb main.c -c 

queue.o: queue.c queue.h
	gcc -Wall -Werror -ggdb queue.c -c

clean:
	rm -f *.o queue