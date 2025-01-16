{
module Lexer where
{

%token

tokens :-

	-- keywords
	"PROGRAM"	{\s -> Program}
	"IS"		{\s -> Is}
	"BEGIN"		{\s -> Begin}
	"END"		{\s -> End}
	"PROCEDURE"	{\s -> Procedure}
	"FOR"		{\s -> For}
	"TO"		{\s -> To}
	"DOWNTO"	{\s -> DownTo}
	"DO"		{\s -> Do}
	"REPEAT"	{\s -> Repeat}
	"UNTIL"		{\s -> Until}
	"IF"		{\s -> If}
	"THEN"		{\s -> Then}
	"ELSE"		{\s -> Else}
	"ENDIF"		{\s -> EndIf}
	"WHILE"		{\s -> While}
	"ENDWHILE"	{\s -> EndWhile}
	"READ"		{\s -> Read}
	"WRITE"		{\s -> Write}
	"HALT"		{\s -> Halt}

	-- ops & symbols
	":="		{\s -> Assign}
	"+"		{\s -> Plus}
	"-"		{\s -> Minus}
	"*"		{\s -> Mul}
	"/"		{\s -> Div}
	"%"		{\s -> Mod}
	"="		{\s -> Eq}
	"!="		{\s -> NEq}
	"<="		{\s -> LEq}
	">="		{\s -> GEq}
	"<"		{\s -> Less}
	">"		{\s -> Greater}
	","		{\s -> Comma}
	";"		{\s -> Semicolon}
	"("		{\s -> LParen}
	")"		{\s -> RParen}
	"["		{\s -> LBracket}
	"]"		{\s -> RBracket}

	-- identifiers
	[_a-z][_a-z0-9]* {\s Identifier s}
	
	-- nums
	[0-9]+		{\s -> Numer (read s) }

	-- comments
	"#" .*		{\_ -> Comment}

	-- white space
	[ \t\n]+	{\_ -> WhiteSpace}

{
data Token
	= Program
	| Is
	| Begin
	| End
	| Procedure
	| For
	| To
	| DownTo
	| Do
	| Repeat
	| Until
	| If
	| Then
	| Else
	| EndIf
	| While
	| EndWhile
	| Read
	| Write
	| Halt	
	| Assign
	| Plus
	| Minus
	| Mul
	| Div
	| Mod
	| Eq
	| NEq
	| LEq
	| GEq
	| Less
	| Greater
	| Comma
	| Semicolon
	| LParen
	| RParen
	| LBracket
	| RBracket
	| Identifier String
	| Number Int
	| Comment
	| WhiteSpace
	deriving (Show, Eq)
}
