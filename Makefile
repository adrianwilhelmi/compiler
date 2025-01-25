FLAGS=-Wall -std=c++17
CXX=g++

parser: parser.y
	bison -d parser.y

lexer: lexer.l
	flex lexer.l

compiler: main.cpp ast.cpp parser.cpp lexer.cpp
	$(CXX) $(FLAGS) -o program main.cpp ast.cpp parser.cpp lexer.cpp -lfl
	
clean:
	rm -f $(LEX_OUT) *.o parser.cpp parser.hpp lexer.cpp lexer.hpp

