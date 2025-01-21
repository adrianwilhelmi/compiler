//author Adrian Wilhelmi

#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include"ast.hpp"
struct ASTVisitor{
	virtual ~ASTVisitor() = default;
	
	virtual void visit(NumberExpr& expr) = 0;
	virtual void visit(VariableExpr& expr) = 0;
	virtual void visit(ArrayAccessExpr& expr) = 0;
	virtual void visit(BinaryOpExpr& expr) = 0;

	virtual void visit(AssignStmt& stmt) = 0;
	virtual void visit(ForStmt& stmt) = 0;
	virtual void visit(RepeatUntilStmt& stmt) = 0;
	virtual void visit(IfStmt& stmt) = 0;
	virtual void visit(ProcedureCallStmt& stmt) = 0;
	virtual void visit(ReadStmt& stmt) = 0;
	virtual void visit(WriteStmt& stmt) = 0;

	virtual void visit(ProcedureDecl& procedure) = 0;
	virtual void visit(Program& program) = 0;
};

#endif //AST_VISITOR_HPP
