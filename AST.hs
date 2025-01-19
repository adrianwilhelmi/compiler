module AST where

data Program
  = Program [Procedure] Main
  deriving (Show)

data Procedure
  = Procedure ProcHead [Declaration] [Command]
  | ProcedureNoDecl ProcHead [Command]
  deriving (Show)

data Main
  = Main [Declarations] [Command]
  | MainNoDecl [Command]
  deriving (Show)

data Declaration
  = VarDecl String
  | ArrayDecl String Int Int
  deriving (Show)

data Command
  = Assign Identifier Expression
  | IfThenEsle Condition [Command] [Command]
  | IfThen Condition [Command]
  | While Condition [Command]
  | RepeatUntil [Command] Condition
  | ForLoop Identifier Value Value [Command]
  | ForLoopDown Identifier Value Value [Command]
  | ProcedureCall String [Identifier]
  | Read Identifier
  | Write Value
  deriving (Show)

data ProcHead
  = ProcHead String [ArgumentDecl]
  deriving (Show)

data Expression
  = ValueExpr Value
  | AddExpr Value Value
  | SubExpr Value Value
  | MulExpr Value Value
  | DivExpr Value Value
  | ModExpr Value Value
  | ProcedureCall String [Value]
  deriving (Show)

data Condition
  = Equal Value Value
  | NotEqual Value Value
  | Greater Value Value
  | Less Value Value
  | GreaterEqual Value Value
  | LessEqual Value Value
  deriving (Show)

data Value
  = NumValue Int
  | IdValue Identifier
  deriving (Show)

data Identifier
  = SimpleId String
  | ArrayId String Identifier
  | ArrayIndexNum String Int
  deriving (Show)



