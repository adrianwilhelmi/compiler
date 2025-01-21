LEX_SRC=lexer.l
LEX_OUT=lexer

FLAGS=-Wall -O3 -Wextra -Werror -std=c++17
LEX_LIBS=-lfl -lstdc++

LEX=flex++
CXX=g++

all: lexer

lexer: lexer.l
	$(LEX) $(LEX_SRC)
	$(CXX) $(FLAGS) lex.yy.cc -o $(LEX_OUT) $(LEX_LIBS)

clean:
	rm -f $(LEX_OUT) *.o *.yy.cc
