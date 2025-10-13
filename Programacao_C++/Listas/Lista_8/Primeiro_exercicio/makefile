CXX = g++
CXXFLAGS = -std=c++11 -Wall
OBJS = Produto.o Consulta.o main.o

all: programa

programa: $(OBJS)
	$(CXX) $(CXXFLAGS) -o programa $(OBJS)

Produto.o: Produto.cpp Produto.h
	$(CXX) $(CXXFLAGS) -c Produto.cpp

Consulta.o: Consulta.cpp Consulta.h Produto.h
	$(CXX) $(CXXFLAGS) -c Consulta.cpp

main.o: main.cpp Produto.h Consulta.h
	$(CXX) $(CXXFLAGS) -c main.cpp

clean:
	rm -f *.o programa
