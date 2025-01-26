/* author: Adrian Wilhelmi */

%option yylineno
%option nounput
%option never-interactive
%option outfile="lexer.cpp"
%option header="lexer.hpp"

%{

#include <FlexLexer.h>

#include"parser.hpp"

#include<string>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<fstream>

using namespace calc;
%}

%%

[ \t\r\n]+	{}
"#".*		{}

PROGRAM		{return Parser::token::TOKEN_PROGRAM;}
IS		{return Parser::token::TOKEN_IS;}
BEGIN		{return Parser::token::TOKEN_BEGIN;}
END		{return Parser::token::TOKEN_END;}
PROCEDURE	{return Parser::token::TOKEN_PROCEDURE;}
FOR		{return Parser::token::TOKEN_FOR;}
ENDFOR		{return Parser::token::TOKEN_ENDFOR;}
FROM		{return Parser::token::TOKEN_FROM;}
TO		{return Parser::token::TOKEN_TO;}
DOWNTO		{return Parser::token::TOKEN_DOWNTO;}
DO		{return Parser::token::TOKEN_DO;}
REPEAT		{return Parser::token::TOKEN_REPEAT;}
UNTIL		{return Parser::token::TOKEN_UNTIL;}
IF		{return Parser::token::TOKEN_IF;}
THEN		{return Parser::token::TOKEN_THEN;}
ELSE		{return Parser::token::TOKEN_ELSE;}
ENDIF		{return Parser::token::TOKEN_ENDIF;}
WHILE		{return Parser::token::TOKEN_WHILE;}
ENDWHILE	{return Parser::token::TOKEN_ENDWHILE;}
READ		{return Parser::token::TOKEN_READ;}
WRITE		{return Parser::token::TOKEN_WRITE;}
HALT		{return Parser::token::TOKEN_HALT;}

":="		{return Parser::token::TOKEN_ASSIGN;}
"+"		{return Parser::token::TOKEN_PLUS;}
"-"		{return Parser::token::TOKEN_MINUS;}
"*"		{return Parser::token::TOKEN_MUL;}
"/"		{return Parser::token::TOKEN_DIV;}
"%"		{return Parser::token::TOKEN_MOD;}
"="		{return Parser::token::TOKEN_EQ;}
"!="		{return Parser::token::TOKEN_NEQ;}
"<="		{return Parser::token::TOKEN_LEQ;}
">="		{return Parser::token::TOKEN_GEQ;}
"<"		{return Parser::token::TOKEN_LESS;}
">"		{return Parser::token::TOKEN_GREATER;}
","		{return Parser::token::TOKEN_COMMA;}
";"		{return Parser::token::TOKEN_SEMICOLON;}
":"		{return Parser::token::TOKEN_COLON;}
"("		{return Parser::token::TOKEN_LPAREN;}
")"		{return Parser::token::TOKEN_RPAREN;}
"["		{return Parser::token::TOKEN_LBRACKET;}
"]"		{return Parser::token::TOKEN_RBRACKET;}

"T "[a-zA-Z_][a-zA-Z0-9_]* {
	yylval->emplace<std::string>(std::string(yytext + 1));
	return Parser::token::TOKEN_TPIDENTIFIER;
}

[a-zA-Z_][a-zA-Z0-9_]*	{
	yylval->emplace<std::string>(std::string(yytext));
	return Parser::token::TOKEN_IDENTIFIER;
}

[0-9]+	{
	yylval->emplace<int64_t>(strtoll(yytext, nullptr, 10));
	return Parser::token::TOKEN_NUMBER;
}

.		{std::cout << "lexer: unexpected character" << yytext << "\n";}

%%

int yywrap(){
	return 1;
}
