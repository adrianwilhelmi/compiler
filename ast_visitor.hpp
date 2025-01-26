//author Adrian Wilhelmi

#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include"ast.hpp"

class ASTVisitor{
public:
	virtual void visit(NumberExpr& expr) = 0;
	virtual void visit(VariableExpr& expr) = 0;
	virtual void visit(ArrayAccessWithIdExpr& expr) = 0;
	virtual void visit(ArrayAccessWithNumExpr& expr) = 0;
	virtual void visit(ArrayDeclarationExpr& expr) = 0;
	virtual void visit(VariableDeclarationExpr& expr) = 0;
	virtual void visit(VariableArgDeclExpr& expr) = 0;
	virtual void visit(ArrayArgDeclExpr& expr) = 0;
	virtual void visit(ProcedureHeadExpr& expr) = 0;
	virtual void visit(BinaryOpExpr& expr) = 0;
	virtual void visit(ConditionExpr& expr) = 0;

	virtual void visit(AssignStmt& stmt) = 0;
	virtual void visit(WhileStmt& stmt) = 0;
	virtual void visit(ForStmt& stmt) = 0;
	virtual void visit(RepeatUntilStmt& stmt) = 0;
	virtual void visit(IfStmt& stmt) = 0;
	virtual void visit(ProcedureCallStmt& stmt) = 0;
	virtual void visit(ReadStmt& stmt) = 0;
	virtual void visit(WriteStmt& stmt) = 0;

	virtual void visit(ProcedureDecl& procedure) = 0;
	virtual void visit(MainProcedure& main) = 0;
	virtual void visit(MainProcedureNoDecl& main) = 0;
	virtual void visit(Program& program) = 0;
};

#endif //AST_VISITOR_HPP
