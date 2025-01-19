{
module Parser where

import Lexer
import AST
}

%name parse
%tokentype {Token}
%error {parseError}

%%

-- program
program_all	: PROCEDURE proc_head IS declarations BEGIN commands END
	    	{ Program [$1] (MainNoDecl $5) }
		| PROGRAM IS declarations BEGIN commands END
		{ Program [] (Main $3 $5) }

-- procedures
procedures	: procedures PROCEDURE proc_head IS declarations BEGIN commands END
	   	{ $1 ++ [Procedure $3 $5 $7] }
		| procedures PROCEDURE proc_head IS BEGIN commands END
		{ $1 ++ [ProcedureNoDecl $3 $5] }

-- main procedure
main		: PROGRAM IS declarations BEGIN commands END
      		{ Main $3 $5 }
		| PROGRAM IS BEGIN commands END
		{ MainNoDecl $4 }

-- command
commands	: commands command
	 	{ $1 ++ [$2] }
		| command
		{ [$1] }

command		: identifier := expression ;
	 	{Assign (SimpleId $1) $3 }
		| IF condition THEN commands ELSE commands ENDIF
		{ IfThenElse $2 $4 $6 }
		| IF condition THEN commands ENDIF
		{ If Then $2 $4 }
		| WHILE condition DO commands ENDWHILE
		{ While $2 $4 }
		| REPEAT commands UNTIL condition ;
		{ RepeatUntil $2 $4 }
		| FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
		{ ForLoopDown $1 $3 $5 $8 }
		| proc_call ;
		{ ProcedureCall $1 $2 }
		| READ identifier ;
		{ Read (SimpleId $2) }
		| WRITE value;
		{ Write $2 }

-- calling a procedure
proc_call	: pidentifier (args)
	  	{ $1 $3 }

-- procedure head
proc_head	: pidentifier (args_decl)
	  	{ ProcHead $1 $3 }

-- declarations
declarations	: declarations , pidentifier
	     	{ $1 ++ [$3] }
		| declarations , pidentifier [ num : num ]
		{ $1 ++ [ArrayDecl $3 (fst $5) (snd $5)] }
		| pidentifier
		{ [VarDecl $1] }
		| pidentifier [ num : num ]
		{ [ArrayDecl $1 (fst $3) (snd $3)] }

-- procedure arguments
args_decl	: args_decl , pidentifier
	  	{ $1 ++ [$3] }
		| args_decl , T pidentifier
		{ $1 ++ [TPIdentifier $3] }
		| pidentifier
		{ [$1] }
		| T pidentifier
		{ [TPIdentifier $2] }

expression	: value
	   	{ ValueExpr $1 }
		| value + value
		{ AddExpr $1 $3 }
		| value - value
		{ SubExpr $1 $3 }
		| value * value
		{ MulExpr $1 $3 }
		| value / value
		{ DivExpr $1 $3 }
		| value % value
		{ ModExpr $1 $3 }

condition	: value = value
	  	{ Equal $1 $3 }
		| value != value
		{ NotEqual $1 $3 }
		| value > value
		{ Greater $1 $3 }
		| value >= value
		{ GreaterEqual $1 $3 }
		| value < value
		{ Less $1 $3 }
		| value <= value
		{ LessEqual $1 $3 }

value		: num
       		{ NumValue $1 }
		| identifier
		{ IdValue (SimpleId $1) }

identifier	: pidentifier
	   	{ $1 }
		| pidentifier [ pidentifier ]
		{ ArrayId $1 $3 }
		| pidentifier [ num ]	
		{ ArrayIndexNum $1 $3 }

num		: Number
     		{ $1 }

{
parseError :: [TokWrap] -> a
parseError [] = error ("parser: unexpected end of input.")
parseError (tok:_) = case token_posn tok of
	AlexPosn _ line col ->
		error $ "parser: parse error at line " ++ show line ++ ", column " ++ show_col ++ ": unexpected token " ++ show tok
}
