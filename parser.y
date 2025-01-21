/*author: Adrian Wilhelmi*/

%{
#include<iostream>
#include<cstdlib>

#include"tokens.hpp"
#include"ast.hpp"

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
	{
		$$ = std::make_unique<Program>(std::move($1), std::move($2));
	}
	;

procedures:
	procedures TOKEN_PROCEDURE proc_head TOKEN_IS declarations TOKEN_BEGIN commands TOKEN_END
	{
		auto proc = std::make_unique<ProcedureDecl>($3, 
							std::move($5), 
							std::move($7));
		$1.push_back(std::move(proc));
		$$ = std::move($1);
	}
	| procedures TOKEN_PROCEDURE proc_head TOKEN_IS TOKEN_BEGIN commands TOKEN_END
	{
		auto proc = std::make_unique<ProcedureDecl>($3,
							std::vector<std::string>(),
							std::move($6));
		$1.push_back(std::move(proc));
		$$ = std::move($1);
	}
	/* inicjalizacja pustej listy procedur */
	|
	{ $$ = std::vector<std::unique_ptr<ProcedureDecl>>(); }
	;

main:
	TOKEN_PROGRAM TOKEN_IS declarations TOKEN_BEGIN commands TOKEN_END
	{
		$$ = std::make_unique<Program>(std::vector<<std::unique_ptr<ProcedureDecl>>(), std::move($3), std::move($5));
	}
	| TOKEN_PROGRAM TOKEN_IS TOKEN_BEGIN commands TOKEN_END
	{
		$$ = std::move_unique<Program>(std::vector<std::unique_ptr<ProcedureDecl>>(), std::vector<std::unique_ptr<Statement>>(), std::move($4));
	}
	;

commands:
	commands command
	{
		$1.push_back(std::move($2));
		$$ = std::move($1);
	}
	| command
	{
		$$ = std::vector<std::unique_ptr<Statement>>({std::move($1)});
	}
	;

command:
	identifier TOKEN_ASSIGN expression TOKEN_SEMICOLON
	{
		$$ = std::make_unique<AssignStmt>($1, std::move($3));
	}
	| TOKEN_IF condition TOKEN_THEN commands TOKEN_ELSE commands TOKEN_ENDIF
	{
		$$ = std::make_unique<IfStmt>(std::move($2), std::move($4), std::move($6));
	}
	| TOKEN_IF condition TOKEN_THEN commands TOKEN_ENDIF
	{
		$$ = std::make_unique<IfStmt>(std::move($2), std::move($4), 
			std::vector<std::unique_ptr<Statement>>());
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
	;

proc_head:
	pidentifier TOKEN_LPAREN args_decl TOKEN_RPAREN{
		//
	}
	;

proc_call:
	pidentifier TOKEN_LPAREN args TOKEN_RPAREN
	{
		$$ = std::make_unique<ProcedureCallStmt>($1, std::move($3));
		//
	}
	;

declarations:
	declarations TOKEN_COMMA pidentifier
	{
		$1.push_back($3);
		$$ = std::move($1);
		//
	}
	| declarations TOKEN_COMMA pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET{
		//
	}
	| pidentifier{
		$$ = std::vector<std::string>( {$1} );
		//
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET{
		//
	}
	;

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
	;

args:
	args TOKEN_COMMA pidentifier{
		//
	}
	| pidentifier{
		//
	}
	;

expression:
	value{
		$$ = std::move($1);
	}
	| value TOKEN_PLUS value
	{
		$$ = std::make_unique<BinaryOpExpr>("+", std::move($1), std::move($3));
	}
	| value TOKEN_MINUS value
	{
		$$ = std::make_unique<BinaryOpExpr>("-" std::move($1), std::move($3));
	}
	| value TOKEN_MUL value
	{
		$$ = std::make_unique<BinaryOpExpr>("*" std::move($1), std::move($3));
	}
	| value TOKEN_DIV value
	{
		$$ = std::make_unique<BinaryOpExpr>("/" std::move($1), std::move($3));
	}
	| value TOKEN_MOD value
	{
		$$ = std::make_unique<BinaryOpExpr>("%" std::move($1), std::move($3));
	}
	;

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
	;

value:
	num{
		$$ = std::make_unique<NumberExpr>($1);
	}
	| identifier{
		$$ = std::move($1);
	}
	;

identifier:
	pidentifier{
		$$ = std::make_unique<VariableExpr>($1);
	}
	| pidentifier TOKEN_LBRACKET pidentifier TOKEN_RBRACKET{
		$$ = std::make_unique<ArrayAccessExpr>($1, std::move($3));
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_RBRACKET{
		//
	}
	;

num:
	TOKEN_NUMBER{
		//
	}
	;

pidentifier:
	TOKEN_IDENTIFIER{
		//
	}
	;

%%

void yyerror(const char*s){
	std::cerr << "parser: blad skladniowy: " << s << " w linii " << yylineno << std::endl;
	exit(1);
}

int main(){
	yyparse();
	return 0;
}
