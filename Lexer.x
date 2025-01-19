{
module Lexer (Token(..), AlexPosn(..), alexScanTokens, token_posn) where
}

%wrapper "posn"

tokens :-

	--whitespace
	$white+		;

	--comments
	"#" .*		;

	--keywords
	"PROGRAM"	{\p _ -> Program p}
	"IS"		{\p _ -> Is p}
	"BEGIN"		{\p _ -> Begin p}
	"END"		{\p _ -> End p}
	"PROCEDURE"	{\p _ -> Procedure p}
	"FOR"		{\p _ -> For p}
	"FROM"		{\p _ -> From p}
	"TO"		{\p _ -> To p}
	"DOWNTO"	{\p _ -> DownTo p}
	"DO"		{\p _ -> Do p}
	"REPEAT"	{\p _ -> Repeat p}
	"UNTIL"		{\p _ -> Until p}
	"IF"		{\p _ -> If p}
	"THEN"		{\p _ -> Then p}
	"ELSE"		{\p _ -> Else p}
	"ENDIF"		{\p _ -> EndIf p}
	"WHILE"		{\p _ -> While p}
	"ENDWHILE"	{\p _ -> EndWhile p}
	"READ"		{\p _ -> Read p}
	"WRITE"		{\p _ -> Write p}
	"HALT"		{\p _ -> Halt p}

	--ops and symbols
	":="		{\p _ -> Assign p}
	"+"		{\p _ -> Plus p}
	"-"		{\p _ -> Minus p}
	"*"		{\p _ -> Mul p}
	"/"		{\p _ -> Div p}
	"%"		{\p _ -> Mod p}
	"="		{\p _ -> Eq p}
	"!="		{\p _ -> NEq p}
	"<="		{\p _ -> LEq p}
	">="		{\p _ -> GEq p}
	"<"		{\p _ -> Less p}
	">"		{\p _ -> Greater p}
	","		{\p _ -> Comma p}
	";"		{\p _ -> Semicolon p}
	"("		{\p _ -> LParen p}
	")"		{\p _ -> RParen p}
	"["		{\p _ -> LBracket p}
	"]"		{\p _ -> RBracket p}
	":"		{\p _ -> Colon p}

	-- table parameter identifier
	"T "[_a-z][_a-z0-9]*	{\p s -> TPIdentifier p (dropWhile (== ' ') (tail s))}

	-- identifiers
	[_a-z][_a-z0-9]*	{\p s -> Identifier p s}

	-- nums
	[0-9]+		{\p s -> Number p (read s::Int)}

{
data Token
	= Program AlexPosn
	| Is AlexPosn
	| Begin AlexPosn
	| End AlexPosn
	| Procedure AlexPosn
	| For AlexPosn
	| From AlexPosn
	| To AlexPosn
	| DownTo AlexPosn
	| Do AlexPosn
	| Repeat AlexPosn
	| Until AlexPosn
	| If AlexPosn
	| Then AlexPosn
	| Else AlexPosn
	| EndIf AlexPosn
	| While AlexPosn
	| EndWhile AlexPosn
	| Read AlexPosn
	| Write AlexPosn
	| Halt AlexPosn
	| Assign AlexPosn
	| Plus AlexPosn
	| Minus AlexPosn
	| Mul AlexPosn
	| Div AlexPosn
	| Mod AlexPosn
	| Eq AlexPosn
	| NEq AlexPosn
	| LEq AlexPosn
	| GEq AlexPosn
	| Less AlexPosn
	| Greater AlexPosn
	| Comma AlexPosn
	| Semicolon AlexPosn
	| LParen AlexPosn
	| RParen AlexPosn
	| LBracket AlexPosn
	| RBracket AlexPosn
	| Colon	AlexPosn
	| TPIdentifier AlexPosn String
	| TDIdentifier AlexPosn String
	| Identifier AlexPosn String
	| Number AlexPosn Int
	deriving (Eq, Show)

-- extract position from token
token_posn (Program p)	   = p
token_posn (Is p)          = p
token_posn (Begin p)       = p
token_posn (End p)         = p
token_posn (Procedure p)   = p
token_posn (For p)         = p
token_posn (From p)	   = p
token_posn (To p)          = p
token_posn (DownTo p)      = p
token_posn (Do p)          = p
token_posn (Repeat p)      = p
token_posn (Until p)       = p
token_posn (If p)          = p
token_posn (Then p)        = p
token_posn (Else p)        = p
token_posn (EndIf p)       = p
token_posn (While p)       = p
token_posn (EndWhile p)    = p
token_posn (Read p)        = p
token_posn (Write p)       = p
token_posn (Halt p)        = p
token_posn (Assign p)      = p
token_posn (Plus p)        = p
token_posn (Minus p)       = p
token_posn (Mul p)         = p
token_posn (Div p)         = p
token_posn (Mod p)         = p
token_posn (Eq p)          = p
token_posn (NEq p)         = p
token_posn (LEq p)         = p
token_posn (GEq p)         = p
token_posn (Less p)        = p
token_posn (Greater p)     = p
token_posn (Comma p)       = p
token_posn (Semicolon p)   = p
token_posn (LParen p)      = p
token_posn (RParen p)      = p
token_posn (LBracket p)    = p
token_posn (RBracket p)    = p
token_posn (Colon p)	   = p
token_posn (TPIdentifier p _) = p
token_posn (TDIdentifier p _) = p
token_posn (Identifier p _) = p
token_posn (Number p _)    = p
}

