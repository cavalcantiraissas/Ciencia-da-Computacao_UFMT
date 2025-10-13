all: main

main: main.o jogo.o pokemon.o utils.o
	g++ -o main main.o jogo.o pokemon.o utils.o -std=c++11

main.o: main.cpp jogo.h
	g++ -c main.cpp -std=c++11

jogo.o: jogo.cpp jogo.h utils.h
	g++ -c jogo.cpp -std=c++11

pokemon.o: pokemon.cpp pokemon.h
	g++ -c pokemon.cpp -std=c++11

utils.o: utils.cpp utils.h pokemon.h
	g++ -c utils.cpp -std=c++11

clean:
	rm -f *.o main
