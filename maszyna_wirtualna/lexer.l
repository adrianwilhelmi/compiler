/*
 * Lexer interpretera maszyny wirtualnej do projektu z JFTT2024
 *
 * Autor: Maciek Gębala
 * http://ki.pwr.edu.pl/gebala/
 * 2024-11-11
*/
%option noyywrap
%option yylineno
%option nounput
%{
#include "parser.hh"
#include "instructions.hh"

int yylex();
%}
%%
\#.*\n          ;
[ \t\r]+        ;
GET             { yylval = GET;    return COM_1; };
PUT             { yylval = PUT;    return COM_1; };
LOADI           { yylval = LOADI;  return COM_1; };
STOREI          { yylval = STOREI; return COM_1; };
LOAD            { yylval = LOAD;   return COM_1; };
STORE           { yylval = STORE;  return COM_1; };
ADDI            { yylval = ADDI;   return COM_1; };
SUBI            { yylval = SUBI;   return COM_1; };
ADD             { yylval = ADD;    return COM_1; };
SUB             { yylval = SUB;    return COM_1; };
SET             { yylval = SET;    return COM_1; };
HALF            { yylval = HALF;   return COM_0; };
RTRN            { yylval = RTRN;   return JUMP_1; };
JUMP            { yylval = JUMP;   return JUMP_1; };
JPOS            { yylval = JPOS;   return JUMP_1; };
JZERO           { yylval = JZERO;  return JUMP_1; };
JNEG            { yylval = JNEG;   return JUMP_1; };
HALT            { yylval = HALT;   return COM_0; };
-?[0-9]+        { yylval = atoll( yytext );  return NUMBER; };
\n              ;
.               return ERROR;
%%
