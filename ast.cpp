#include"ast.hpp"
#include"ast_visitor.hpp"

void NumberExpr::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void VariableExpr::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void ArrayAccessExpr::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void BinaryOpExpr::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void AssignStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void ForStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void RepeatUntilStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void IfStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void ProcedureCallStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void ReadStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void WriteStmt::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void ProcedureDecl::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}

void Program::accept(ASTVisitor& visitor){
	visitor.visit(*this);
}
