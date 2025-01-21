//author: Adrian Wilhelmi

#ifndef AST_HPP
#define AST_HPP

#include<iostream>
#include<vector>
#include<string>
#include<iomanip>

struct ASTNode{
	virtual ~ASTNode() = default;
	//wzorzec visitora dla generowania kodu i analiz
	virtual void accept(ASTVisitor& visitor) = 0;	
};





/*expression*/
struct Expression : ASTNode {};

struct NumberExpr : Expression{
	int value;
	NumberExpr(int val) : value(val) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this); }
};

struct VariableExpr : Expression {
	std::string name;
	VariableExpr(const std::string& var) : name(var) {}
	void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct ArrayAccesExpr : Expression {
	std::string array_name;
	Expression*index;
	ArrayAccessExpr(const std::string&name, Expression*idx)
		: array_name(name), index(idx) {}
	void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};

struct BinaryOpExpr : Expression{
	std::string op;
	Expression* left;
	Expression* right;
	BinaryOpExpr(const std::string&opr, Expression*lhs, Expression*rhs)
		: op(opr), left(lhs), right(rhs) {}
	void accept(ASTVisitor& visitor) override { visitor.visit(*this); }
};




/*statements*/
struct Statement : ASTNode{};

struct AssignStmt : Statement{
	std::string variable;
	Expression* value;
	AssignStmt(const std::string& var, Expression* val) 
		: variable(var), value(val) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};

struct ForStmt : Statement{
	std::string iterator;
	Expression*from;
	Expression*to;
	bool downto;
	std::vector<Statement*> body;
	ForStmt(const std::string& iter, Expression*start, Expression*end, bool down)
		: iterator(iter), from(start), to(end), downto(down) {}
	void accept(ASTVisitor&visitor) override {visitor.visit(*this);}
};

struct RepeatUntilStmt : Statement{
	std::vector<Statement*> body;
	Expression*condition;
	RepeatUntilStmt(const std::vector<Statement*>&b, Expression*cond)
		: body(b), condition(cond) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};

struct IfStmt : Statement{
	Expression*condition;
	std::vector<Statement*> then_body;
	std::vector<Statement*> else_body;
	IfStmt(Expression*cond, const std::vector<Statement*>& then_b,
		const std::vector<Statement*>& else_b = {})
		: condition(cond), then_body(then_b), else_body(else_b) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};

struct ProcedureCallStmt : Statement{
	std::string name;
	std::vector<Expression*> arguments;
	ProcedureCallStmt(const std::string& proc_name, 
			const std::vector<Expression*>& args)
		: name(proc_name), arguments(args) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};

struct ReadStmt : Statement{
	std::string variable;
	ReadStmt(const std::string& var) : variable(var) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};

struct WriteStmt : Statement{
	Expression*value;
	WriteStmt(Expression*val) : value(val) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};




//procedures and main program
struct ProcedureDecl : ASTNode{
	std::string name;
	std::vector<std::string> parameters;
	std::vector<Statement*> body;
	ProcedureDecl(const std::string& proc_name, 
			const std::vector<std::string>& params,
			const std::vector<Statement*>& b)
		: name(proc_name), parameters(params), body(b) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};
	
struct Program : ASTNode{
	std::vector<ProcedureDecl*> procedures;
	std::vector<Statement*> main_body;
	Program(const std::vector<ProcedureDecl*>& procs, 
		const std::vector<Statement*> body)
		: procedures(procs), main_body(body) {}
	void accept(ASTVisitor& visitor) override {visitor.visit(*this);}
};


#endif //AST_HPP
