# Makefile

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -c -std=c++17

# Source files
SRC = ./src/lexer/lexer.cpp \
      ./src/parser/parser.cpp \
      ./src/assembly/acg.cpp \
      ./src/intermediate/icg.cpp \
      ./src/symboltable/symbTable.cpp \
      ./main.cpp

# Object files
OBJ = ./bin/lexer.o \
      ./bin/parser.o \
      ./bin/acg.o \
      ./bin/icg.o \
      ./bin/symbTable.o \
      ./bin/main.o

# Executable name
TARGET = main.exe

# Rule to build the target
$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET)

# Rule to compile lexer.cpp into lexer.o
./bin/lexer.o: ./src/lexer/lexer.cpp
	$(CXX) $(CXXFLAGS) ./src/lexer/lexer.cpp -o ./bin/lexer.o

# Rule to compile parser.cpp into parser.o
./bin/parser.o: ./src/parser/parser.cpp
	$(CXX) $(CXXFLAGS) ./src/parser/parser.cpp -o ./bin/parser.o

# Rule to compile acg.cpp into acg.o
./bin/acg.o: ./src/assembly/acg.cpp
	$(CXX) $(CXXFLAGS) ./src/assembly/acg.cpp -o ./bin/acg.o

# Rule to compile icg.cpp into icg.o
./bin/icg.o: ./src/intermediate/icg.cpp
	$(CXX) $(CXXFLAGS) ./src/intermediate/icg.cpp -o ./bin/icg.o

# Rule to compile symbTable.cpp into symbTable.o
./bin/symbTable.o: ./src/symboltable/symbTable.cpp
	$(CXX) $(CXXFLAGS) ./src/symboltable/symbTable.cpp -o ./bin/symbTable.o

# Rule to compile main.cpp into main.o
./bin/main.o: ./main.cpp
	$(CXX) $(CXXFLAGS) ./main.cpp -o ./bin/main.o

# Rule to clean the build directory
clean:
	rm -f $(OBJ) $(TARGET)
