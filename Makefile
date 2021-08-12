$CC = gcc

build:
	gcc -Wall ./src/*.c -std=c99 -lSDL2 -o game_binary

run:
	./game_binary

clean:
	rm game_binary

