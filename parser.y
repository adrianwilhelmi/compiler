%{
#include<iostream>
#include<cstdlib>
#include"tokens.hpp"

extern "C" int yylex();
extern "C" int yylineno;
extern "C" char*yytext;

void yyerror(const char*s);
%}

%union{
	int num;
	char*str;
}

%token <num> TOKEN_NUMBER
%token <str> TOKEN_IDENTIFIER
%token <str> TOKEN_TPIDENTIFIER
%token TOKEN_PROGRAM TOKEN_IS TOKEN_BEGIN TOKEN_END
%token TOKEN_PROCEDURE TOKEN_FOR TOKEN_ENDFOR TOKEN_FROM
%token TOKEN_TO TOKEN_DOWNTO TOKEN_DO TOKEN_REPEAT TOKEN_UNTIL
%token TOKEN_IF TOKEN_THEN TOKEN_ELSE TOKEN_ENDIF
%token TOKEN_WHILE TOKEN_ENDWHILE
%token TOKEN_READ TOKEN_WRITE TOKEN_HALT
%token TOKEN_ASSIGN TOKEN_PLUS TOKEN_MINUS TOKEN_MUL TOKEN_DIV
%token TOKEN_MOD TOKEN_EQ TOKEN_NEQ TOKEN_LEQ TOKEN_GEQ
%token TOKEN_LESS TOKEN_GREATER TOKEN_COMMA TOKEN_SEMICOLON
%token TOKEN_COLON TOKEN_LPAREN TOKEN_RPAREN TOKEN_LBRACKET
%token TOKEN_RBRACKET

%start program_all

%%

program_all:
	procedures main

procedures:
	procedures TOKEN_PROCEDURE proc_head TOKEN_IS declarations TOKEN_BEGIN commands TOKEN_END{
		//
	}
	| procedures TOKEN_PROCEDURE proc_head TOKEN_IS TOKEN_BEGIN commands TOKEN_END{
		//
	}
	
main:
	TOKEN_PROGRAM TOKEN_IS declarations TOKEN_BEGIN commands TOKEN_END{
		//
	}
	| TOKEN_PROGRAM TOKEN_IS TOKEN_BEGIN commands TOKEN_END{
		//
	}
	
commands:
	commands command
	| command

command:
	identifier TOKEN_ASSIGN expression TOKEN_SEMICOLON{
		//
	}
	| TOKEN_IF condition TOKEN_THEN commands TOKEN_ELSE commands TOKEN_ENDIF{
		//
	}
	| TOKEN_IF condition TOKEN_THEN commands TOKEN_ENDIF{
		//
	}
	| TOKEN_WHILE condition TOKEN_DO commands TOKEN_ENDWHILE{
		//
	}
	| TOKEN_REPEAT commands TOKEN_UNTIL condition TOKEN_SEMICOLON{
		//
	}
	| TOKEN_FOR pidentifier TOKEN_FROM value TOKEN_TO value TOKEN_DO commands TOKEN_ENDFOR{
		//
	}
	| TOKEN_FOR pidentifier TOKEN_FROM value TOKEN_DOWNTO value TOKEN_DO commands TOKEN_ENDFOR{
		//
	}
	| proc_call TOKEN_SEMICOLON{
		//
	}
	| TOKEN_READ identifier TOKEN_SEMICOLON{
		//
	}
	| TOKEN_WRITE value TOKEN_SEMICOLON{
		//
	}

proc_head:
	pidentifier TOKEN_LPAREN args_decl TOKEN_RPAREN{
		//
	}

proc_call:
	pidentifier TOKEN_LPAREN args TOKEN_RPAREN{
		//
	}

declarations:
	declarations TOKEN_COMMA pidentifier{
		//
	}
	| declarations TOKEN_COMMA pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET{
		//
	}
	| pidentifier{
		//
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET{
		//
	}

args_decl:
	args_decl TOKEN_COMMA pidentifier{
		//
	}
	| args_decl TOKEN_COMMA TOKEN_TPIDENTIFIER{
		//
	}
	| pidentifier{
		//
	}
	| TOKEN_TPIDENTIFIER{
		//
	}

args:
	args TOKEN_COMMA pidentifier{
		//
	}
	| pidentifier{
		//
	}

expression:
	value{
		//
	}
	| value TOKEN_PLUS value{
		//
	}
	| value TOKEN_MINUS value{
		//
	}
	| value TOKEN_MUL value{
		//
	}
	| value TOKEN_DIV value{
		//
	}
	| value TOKEN_MOD value{
		//
	}

condition:
	value TOKEN_EQ value{
		//
	}
	| value TOKEN_NEQ value{
		//
	}
	| value TOKEN_GREATER value{
		//
	}
	| value TOKEN_LESS value{
		//
	}
	| value TOKEN_GEQ value{
		//
	}
	| value TOKEN_LEQ value{
		//
	}

value:
	num{
		//
	}
	| identifier{
		//
	}

identifier:
	pidentifier{
		//
	}
	| pidentifier TOKEN_LBRACKET pidentifier TOKEN_RBRACKET{
		//
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_RBRACKET{
		//
	}
	
num:
	TOKEN_NUMBER{
		//
	}

pidentifier:
	TOKEN_IDENTIFIER{
		//
	}

%%

void yyerror(const char*s){
	std::cerr << "parser: blad skladniowy: " << s << " w linii " << yylineno << std::endl;
	exit(1);
}

int main(){
	yyparse();
	return 0;
}
