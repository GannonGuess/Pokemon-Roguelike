all: game

game: world.c
	gcc world.c movement.c -o game -lm -lncurses

clean:
	rm -rf game *.dSYM
