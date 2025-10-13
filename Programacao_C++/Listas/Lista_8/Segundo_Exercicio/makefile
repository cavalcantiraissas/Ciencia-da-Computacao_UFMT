CXX = g++
CXXFLAGS = -std=c++11 -Wall
OBJS = Item.o Eletronico.o Vestuario.o main.o

all: programa

programa: $(OBJS)
	$(CXX) $(CXXFLAGS) -o programa $(OBJS)

Item.o: Item.cpp Item.h
	$(CXX) $(CXXFLAGS) -c Item.cpp

Eletronico.o: Eletronico.cpp Eletronico.h Item.h
	$(CXX) $(CXXFLAGS) -c Eletronico.cpp

Vestuario.o: Vestuario.cpp Vestuario.h Item.h
	$(CXX) $(CXXFLAGS) -c Vestuario.cpp

main.o: main.cpp Item.h Eletronico.h Vestuario.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o programa
