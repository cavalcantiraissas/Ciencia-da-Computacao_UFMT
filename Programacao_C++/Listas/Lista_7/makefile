# Makefile para compilar e executar o projeto de gerenciamento de biblioteca

# Compilador
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# Arquivos fontes e objetos
SRCS = main.cpp livro.cpp membro.cpp biblioteca.cpp
OBJS = $(SRCS:.cpp=.o)

# Nome do executável
TARGET = biblioteca

# Regra padrão para compilar o programa
all: $(TARGET)

# Regra para criar o executável a partir dos arquivos objeto
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regra para compilar arquivos fonte em arquivos objeto
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra para limpar arquivos objeto e o executável
clean:
	rm -f $(OBJS) $(TARGET)

# Regra para compilar e limpar
rebuild: clean all

# Regra para executar o programa após a compilação
run: all
	./$(TARGET)

.PHONY: all clean rebuild run

