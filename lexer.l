%{
#include<string>
#include<cstring>
#include<cstdlib>
#include<iostream>
#include<fstream>

#include"tokens.hpp"

//extern "C" int yylex();

%}

%option noyywrap
%option yylineno

%%

[ \t\r\n]+	{}
"#".*		{}

PROGRAM		{return TOKEN_PROGRAM;}
IS		{return TOKEN_IS;}
BEGIN		{return TOKEN_BEGIN;}
END		{return TOKEN_END;}
PROCEDURE	{return TOKEN_PROCEDURE;}
FOR		{return TOKEN_FOR;}
ENDFOR		{return TOKEN_ENDFOR;}
FROM		{return TOKEN_FROM;}
TO		{return TOKEN_TO;}
DOWNTO		{return TOKEN_DOWNTO;}
DO		{return TOKEN_DO;}
REPEAT		{return TOKEN_REPEAT;}
UNTIL		{return TOKEN_UNTIL;}
IF		{return TOKEN_IF;}
THEN		{return TOKEN_THEN;}
ELSE		{return TOKEN_ELSE;}
ENDIF		{return TOKEN_ENDIF;}
WHILE		{return TOKEN_WHILE;}
ENDWHILE	{return TOKEN_ENDWHILE;}
READ		{return TOKEN_READ;}
WRITE		{return TOKEN_WRITE;}
HALT		{return TOKEN_HALT;}

":="		{return TOKEN_ASSIGN;}
"+"		{return TOKEN_PLUS;}
"-"		{return TOKEN_MINUS;}
"*"		{return TOKEN_MUL;}
"/"		{return TOKEN_DIV;}
"%"		{return TOKEN_MOD;}
"="		{return TOKEN_EQ;}
"!="		{return TOKEN_NEQ;}
"<="		{return TOKEN_LEQ;}
">="		{return TOKEN_GEQ;}
"<"		{return TOKEN_LESS;}
">"		{return TOKEN_GREATER;}
","		{return TOKEN_COMMA;}
";"		{return TOKEN_SEMICOLON;}
":"		{return TOKEN_COLON;}
"("		{return TOKEN_LPAREN;}
")"		{return TOKEN_RPAREN;}
"["		{return TOKEN_LBRACKET;}
"]"		{return TOKEN_RBRACKET;}

"T "[a-zA-Z_][a-zA-Z0-9_]* {
	yylval.str = strdup(yytext + 1);
	return TOKEN_TPIDENTIFIER;
}

[a-zA-Z_][a-zA-Z0-9_]*	{
	yylval.str = strdup(yytext);
	return TOKEN_IDENTIFIER;
}

[0-9]+	{
	yylval.num = atoi(yytext);
	return TOKEN_NUMBER;
}

.		{std::cout << "lexer: unexpected character '" << yytext << "' at line " << yylineno << "\n";}

%%

/*
int main(int argc, char*argv[]){
	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " <filename>\n";
		return 1;
	}

	std::ifstream file(argv[1]);
	if(!file){
		std::cerr << "couldnt open the file\n";
		return 1;
	}

	yyFlexLexer lexer(&file);
	while(lexer.yylex());


	return 0;
}
*/
