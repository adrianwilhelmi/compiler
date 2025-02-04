FLAGS=-Wall -std=c++17
CXX=g++

all: parser lexer compiler

parser: parser.y
	bison -d parser.y

lexer: lexer.l
	flex lexer.l

compiler: main.cpp ast.cpp parser.cpp lexer.cpp
	$(CXX) $(FLAGS) -o kompilator main.cpp ast.cpp parser.cpp lexer.cpp -lfl
	
clean:
	rm -f *.o parser.cpp parser.hpp lexer.cpp lexer.hpp

