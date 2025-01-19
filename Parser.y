{
module Parser where

import Lexer
import AST
}

%name parse
%tokentype { Token }
%error { parseError }

%token
	Procedure	{ Procedure $$ }
	Program		{ Program $$ }
	Is		{ Is $$ }
	Begin		{ Begin $$ }
	End		{ End $$ }
	Assign		{ Assign $$ }
	If		{ If $$ }
	Then		{ Then $$ }
	Else		{ Else $$ }
	EndIf		{ EndIf $$ }
	While		{ While $$ }
	EndWhile	{ EndWhile $$ }
	Repeat		{ Repeat $$ }
	Until		{ Until $$ }
	From		{ From $$ }
	To		{ To $$ }
	DownTo		{ DownTo $$ }
	Do		{ Do $$ }
	For		{ For $$ }
	EndFor		{ EndFor $$ }
	Read		{ Read $$ }
	Write		{ Write $$ }
	Semicolon	{ Semicolon $$ }
	LParen		{ LParen $$ }
	RParen		{ RParen $$ }
	Colon		{ Colon $$ }
	Comma		{ Comma $$ }
	Plus		{ Plus $$ }
	Minus		{ Minus $$ }
	Mul		{ Mul $$ }
	Div		{ Div $$ }
	Mod		{ Mod $$ }
	Eq		{ Eq $$ }
	NEq		{ NEq $$ }
	Greater		{ Greater $$ }
	GEq		{ GEq $$ }
	Less		{ Less $$ }
	LEq		{ LEq $$ }
	LBracket	{ LBracket $$ }
	RBracket	{ RBracket $$ }
	TPIdentifier	{ TPIdentifier $$1 $$2 }
	Identifier	{ Identifier $$1 $$2 }
	Number		{ Number $$1 $$2 }

%%

-- program
program_all	: procedures main
	    	{ Program $1 $2 }

-- procedures
procedures	: procedures Procedure proc_head Is declarations Begin commands End
	   	{ $1 ++ [Procedure $3 $5 $7] }
		| procedures Procedure proc_head Is Begin commands End
		{ $1 ++ [ProcedureNoDecl $3 $5] }

-- main procedure
main		: Program Is declarations Begin commands End
      		{ Main $3 $5 }
		| Program Is Begin commands End
		{ MainNoDecl $4 }

-- command
commands	: commands command
	 	{ $1 ++ [$2] }
		| command
		{ [$1] }

command		: identifier Assign expression Semicolon
	 	{ Assign (SimpleId $1) $3 }
		| If condition Then commands Else commands EndIf
		{ IfThenElse $2 $4 $6 }
		| If condition Then commands EndIf
		{ IfThen $2 $4 }
		| While condition Do commands EndWhile
		{ While $2 $4 }
		| Repeat commands Until condition Semicolon
		{ RepeatUntil $2 $4 }
		| For pidentifier From value To value Do commands EndFor
		{ ForLoop $1 $3 $5 $8 }
		| For pidentifier From value DownTo value Do commands EndFor
		{ ForLoopDown $1 $3 $5 $8 }
		| proc_call Semicolon
		{ ProcedureCall $1 $2 }
		| Read identifier Semicolon
		{ Read (SimpleId $2) }
		| Write value Semicolon
		{ Write $2 }

-- procedure head
proc_head	: pidentifier LParen args_decl RParen
	  	{ ProcHead $1 $3 }

-- calling a procedure
proc_call	: pidentifier LParen args RParen
	  	{ ProcedureCall $1 $3 }

-- declarations
declarations	: declarations Comma pidentifier
	     	{ $1 ++ [$3] }
		| declarations Comma pidentifier LBracket num Colon num RBracket
		{ $1 ++ [ArrayDecl $3 $5 $7] }
		| pidentifier
		{ [VarDecl $1] }
		| pidentifier LBracket num Colon num RBracket
		{ [ArrayDecl $1 $3 $5] }

-- procedure arguments
args_decl	: args_decl Comma pidentifier
	  	{ $1 ++ [$3] }
		| args_decl Comma TPIdentifier
		{ $1 ++ [$3] }
		| pidentifier
		{ [$1] }
		| TPIdentifier
		{ [$1] }

args		: args Comma pidentifier
      		{ $1 ++ [$3] }
      		| pidentifier
		{ [$1] }

expression	: value
	   	{ ValueExpr $1 }
		| value Plus value
		{ AddExpr $1 $3 }
		| value Minus value
		{ SubExpr $1 $3 }
		| value Mul value
		{ MulExpr $1 $3 }
		| value Div value
		{ DivExpr $1 $3 }
		| value Mod value
		{ ModExpr $1 $3 }

condition	: value Eq value
	  	{ Equal $1 $3 }
		| value NEq value
		{ NotEqual $1 $3 }
		| value Greater value
		{ Greater $1 $3 }
		| value GEq value
		{ GreaterEqual $1 $3 }
		| value Less value
		{ Less $1 $3 }
		| value LEq value
		{ LessEqual $1 $3 }

value		: num
       		{ NumValue $1 }
		| identifier
		{ IdValue (SimpleId $1) }

identifier	: pidentifier
	   	{ SimpleId $1 }
		| pidentifier LBracket pidentifier RBracket
		{ ArrayId $1 $3 }
		| pidentifier LBracket num RBracket
		{ ArrayIndexNum $1 $3 }

num		: Number
     		{ $1 }

pidentifier	: Identifier
	    	{ SimpleId $1 }

{
parseError :: [TokWrap] -> a
parseError [] = error ("parser: unexpected end of input.")
parseError (tok:_) = case token_posn tok of
	AlexPosn _ line col ->
		error $ "parser: parse error at line " ++ show line ++ ", column " ++ show col ++ ": unexpected token " ++ show tok
}
