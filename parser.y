/*author: Adrian Wilhelmi*/

%{
#include<iostream>
#include<cstdlib>
#include<string>
#include<memory>
#include<vector>

#include"tokens.hpp"
#include"ast.hpp"

extern "C" int yylex();
extern "C" int yylineno;
extern "C" char*yytext;

void yyerror(const char*s);
%}

%union{
	int num;
	std::string str;
	std::unique_ptr<Main> main_proc;
	std::unique_ptr<Program> program;
	std::unique_ptr<ProcedureDecl> proc_decl;
	std::unique_ptr<Statement> stmt;
	std::unique_ptr<Expression> expr;
	std::vector<std::unique_ptr<ProcedureDecl>> proc_vec;
	std::vector<std::unique_ptr<Statement>> stmt_vec;
	std::vector<std::unique_ptr<Expression>> expr_vec;
}

%type <main_proc> main
%type <expr_vec> declarations
%type <program> program_all
%type <proc_vec> procedures
%type <proc_decl> proc_head
%type <stmt_vec> commands
%type <stmt> command
%type <stmt> proc_call
%type <expr_vec> args
%type <expr_vec> args_decl
%type <expr> expression
%type <expr> condition
%type <expr> value
%type <expr> identifier
%type <num> num
%type <str> pidentifier

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
	|
	{
		//
	}
	;

main:
	TOKEN_PROGRAM TOKEN_IS declarations TOKEN_BEGIN commands TOKEN_END
	{
		$$ = std::make_unique<MainProcedure>(std::move($3), std::move($5));
	}
	| TOKEN_PROGRAM TOKEN_IS TOKEN_BEGIN commands TOKEN_END
	{
		$$ = std::make_unique<MainProcedureNoDecl>(std::move($4));
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
		$$ = std::vector<std::unique_ptr<Statement>>();
		$$.push_back(std::move($1));
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
		
		$$ = std::make_unique<WhileStmt>(std::move($2), std::move($4));
	}
	| TOKEN_REPEAT commands TOKEN_UNTIL condition TOKEN_SEMICOLON
	{
		$$ = std::make_unique<RepeatUntilStmt>(std::move($2), std::move($4));
	}
	| TOKEN_FOR pidentifier TOKEN_FROM value TOKEN_TO value TOKEN_DO commands TOKEN_ENDFOR
	{
		$$ = std::make_unique<ForStmt>(std::move($2), std::move($4),
					std::move($6), std::move($8), false);
	}
	| TOKEN_FOR pidentifier TOKEN_FROM value TOKEN_DOWNTO value TOKEN_DO commands TOKEN_ENDFOR{
		$$ = std::make_unique<ForStmt>(std::move($2), std::move($4),
					std::move($6), std::move($8), true);
	}
	| proc_call TOKEN_SEMICOLON
	{
		$$ = std::move($1);
	}
	| TOKEN_READ identifier TOKEN_SEMICOLON
	{
		$$ = std::make_unique<ReadStmt>(std::move($2));
	}
	| TOKEN_WRITE value TOKEN_SEMICOLON
	{
		$$ = std::make_unique<WriteStmt>(std::move($2));
	}
	;

proc_head:
	pidentifier TOKEN_LPAREN args_decl TOKEN_RPAREN
	{
		$$ = std::make_unique<ProcedureHeadExpr>(std::move($1), 
							std::move($3));
	}
	;

proc_call:
	pidentifier TOKEN_LPAREN args TOKEN_RPAREN
	{
		$$ = std::make_unique<ProcedureCallStmt>(std::move($1), std::move($3));
	}
	;

declarations:
	declarations TOKEN_COMMA pidentifier
	{
		$1.push_back(std::move($3));
		$$ = std::move($1);
	}
	| declarations TOKEN_COMMA pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET
	{
		auto arr_decl = std::make_unique<ArrayDeclarationExpr>($3, $5, $7);
		$1.push_back(std::move(arr_decl));
		$$ = std::move($1);
	}
	| pidentifier
	{
		$$ = std::vector<unique_ptr<Expression>>();
		$$.push_back(std::move($1));
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_COLON num TOKEN_RBRACKET
	{
		auto decl = std::make_unique<ArrayDeclarationExpr>(std::move($1),
							std::move($3), std::move($5));
		$$ = std::vector<unique_ptr<Expression>>();
		$$.push_back(decl);
	}
	;

args_decl:
	args_decl TOKEN_COMMA pidentifier
	{
		$1.push_back(std::move($3));
		$$ = std::move($1);
	}
	| args_decl TOKEN_COMMA TOKEN_TPIDENTIFIER
	{
		$1.push_back(std::move($3));
		$$ = std::move($1);
	}
	| pidentifier
	{
		$$ = std::vector<std::unique_ptr<Expression>>();
		$$.push_back(std::move($1));
	}
	| TOKEN_TPIDENTIFIER
	{
		$$ = std::vector<std::unique_ptr<Expression>>();
		$$.push_back(std::move($1));
	}
	;

args:
	args TOKEN_COMMA pidentifier
	{
		$1.push_back(std::move($3));
		$$ = $1;
	}
	| pidentifier
	{
		$$ = std::vector<std::unique_ptr<Expression>>();
		$$.push_back(std::move($1));
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
		$$ = std::make_unique<BinaryOpExpr>("-", std::move($1), std::move($3));
	}
	| value TOKEN_MUL value
	{
		$$ = std::make_unique<BinaryOpExpr>("*", std::move($1), std::move($3));
	}
	| value TOKEN_DIV value
	{
		$$ = std::make_unique<BinaryOpExpr>("/", std::move($1), std::move($3));
	}
	| value TOKEN_MOD value
	{
		$$ = std::make_unique<BinaryOpExpr>("%", std::move($1), std::move($3));
	}
	;

condition:
	value TOKEN_EQ value
	{
		$$ = std::make_unique<ConditionExpr>("=", std::move($1), std::move($3));
	}
	| value TOKEN_NEQ value
	{
		$$ = std::make_unique<ConditionExpr>("!=", std::move($1), std::move($3));
	}
	| value TOKEN_GREATER value
	{
		$$ = std::make_unique<ConditionExpr>(">", std::move($1), std::move($3));
	}
	| value TOKEN_LESS value
	{
		$$ = std::make_unique<ConditionExpr>("<", std::move($1), std::move($3));
	}
	| value TOKEN_GEQ value
	{
		$$ = std::make_unique<ConditionExpr>(">=", std::move($1), std::move($3));
	}
	| value TOKEN_LEQ value
	{
		$$ = std::make_unique<ConditionExpr>("<=", std::move($1), std::move($3));
	}
	;

value:
	num
	{
		$$ = std::make_unique<NumberExpr>($1);
	}
	| identifier
	{
		$$ = std::move($1);
	}
	;

identifier:
	pidentifier
	{
		$$ = std::make_unique<VariableExpr>($1);
	}
	| pidentifier TOKEN_LBRACKET pidentifier TOKEN_RBRACKET
	{
		$$ = std::make_unique<ArrayAccessWithIdExpr>($1, $3);
	}
	| pidentifier TOKEN_LBRACKET num TOKEN_RBRACKET
	{
		$$ = std::make_unique<ArrayAccessWithNumExpr>($1, $3);
	}
	;

num:
	TOKEN_NUMBER
	{
		$$ = $1;
	}
	;

pidentifier:
	TOKEN_IDENTIFIER
	{
		$$ = $1;
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
