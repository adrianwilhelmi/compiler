//author: Adrian Wilhelmi

#ifndef AST_HPP
#define AST_HPP

#include<memory>
#include<vector>
#include<string>
#include<iomanip>

class ASTVisitor;

class ASTNode{
public:
	//wzorzec visitora dla generowania kodu i analiz
	virtual void accept(ASTVisitor& visitor) = 0;	
	virtual ~ASTNode() = default;
};





/*expression*/
class Expression : public ASTNode {};

class NumberExpr : public Expression{
public:
	int64_t value;
	NumberExpr(int val) : value(val) {}
	void accept(ASTVisitor& visitor) override;
};

class VariableExpr : public Expression {
public:
	std::string name;
	VariableExpr(const std::string& var) : name(var) {}
	void accept(ASTVisitor& visitor) override;
};

class ArrayAccessWithIdExpr : public Expression {
public:
	std::string array_name;
	std::string index;
	ArrayAccessWithIdExpr(const std::string&name, std::string idx)
		: array_name(name), index(idx) {}
	void accept(ASTVisitor& visitor) override;
};

class ArrayAccessWithNumExpr : public Expression {
public:
	std::string array_name;
	int64_t index;
	ArrayAccessWithNumExpr(const std::string&name, int64_t idx)
		: array_name(name), index(idx) {}
	void accept(ASTVisitor& visitor) override;
};

class ArrayDeclarationExpr : public Expression {
public:
	std::string var;
	int64_t from;
	int64_t to;

	ArrayDeclarationExpr(std::string variable,
				int64_t from, int64_t to)
		: var(variable), from(from), to(to) {}
	void accept(ASTVisitor& visitor) override;	
};

class VariableDeclarationExpr : public Expression {
public:
	std::string var;

	VariableDeclarationExpr(std::string variable)
		: var(variable) {}
	void accept(ASTVisitor& visitor) override;	
};


class ProcedureHeadExpr : public Expression {
public:
	std::string proc_name;
	std::vector<std::string> vars;

	ProcedureHeadExpr(const std::string& name,
			std::vector<std::string> variables)
		: proc_name(name), vars(std::move(variables)) {}
	void accept(ASTVisitor& visitor) override;	
};

class BinaryOpExpr : public Expression{
public:
	std::string op;
	std::unique_ptr<Expression> left;
	std::unique_ptr<Expression> right;
	BinaryOpExpr(const std::string&opr, std::unique_ptr<Expression> lhs, 
			std::unique_ptr<Expression>rhs)
		: op(opr), left(std::move(lhs)), right(std::move(rhs)) {}
	void accept(ASTVisitor& visitor) override;
};

class ConditionExpr : public Expression{
public:
	std::string op;
	std::unique_ptr<Expression> left;
	std::unique_ptr<Expression> right;
	ConditionExpr(const std::string&opr, std::unique_ptr<Expression> lhs,
			std::unique_ptr<Expression>rhs)
		: op(opr), left(std::move(lhs)), right(std::move(rhs)) {}
	void accept(ASTVisitor& visitor) override;
};




/*statements*/
class Statement : public ASTNode{};

class AssignStmt : public Statement{
public:
	std::unique_ptr<Expression> variable;
	std::unique_ptr<Expression> value;
	AssignStmt(std::unique_ptr<Expression> var, std::unique_ptr<Expression> val) 
		: variable(std::move(var)), value(std::move(val)) {}
	void accept(ASTVisitor& visitor) override;
};

class WhileStmt : public Statement{
public:
	std::vector<std::unique_ptr<Statement>> body;
	std::unique_ptr<Expression> condition;
	WhileStmt(std::unique_ptr<Expression> cond,
			std::vector<std::unique_ptr<Statement>> b)
		: body(std::move(b)), condition(std::move(cond)) {}
	void accept(ASTVisitor& visitor) override;
};

class ForStmt : public Statement{
public:
	std::string iterator;
	std::unique_ptr<Expression> from;
	std::unique_ptr<Expression> to;
	std::vector<std::unique_ptr<Statement>> body;
	bool downto;
	ForStmt(const std::string& iter, 
			std::unique_ptr<Expression> start, 
			std::unique_ptr<Expression> end, 
			std::vector<std::unique_ptr<Statement>> b,
			bool down)
		: iterator(iter), from(std::move(start)), to(std::move(end)), 
			body(std::move(b)), downto(down) {}
	void accept(ASTVisitor& visitor) override;
};

class RepeatUntilStmt : public Statement{
public:
	std::vector<std::unique_ptr<Statement>> body;
	std::unique_ptr<Expression> condition;
	RepeatUntilStmt(std::vector<std::unique_ptr<Statement>> b, 
			std::unique_ptr<Expression> cond)
		: body(std::move(b)), condition(std::move(cond)) {}
	void accept(ASTVisitor& visitor) override;
};

class IfStmt : public Statement{
public:
	std::unique_ptr<Expression> condition;
	std::vector<std::unique_ptr<Statement>> then_body;
	std::vector<std::unique_ptr<Statement>> else_body;
	IfStmt(std::unique_ptr<Expression> cond, 
			std::vector<std::unique_ptr<Statement>> then_b,
			std::vector<std::unique_ptr<Statement>> else_b)
		: condition(std::move(cond)), then_body(std::move(then_b)), else_body(std::move(else_b)) {}
	void accept(ASTVisitor& visitor) override;
};

class ProcedureCallStmt : public Statement{
public:
	std::string name;
	std::vector<std::string> arguments;
	ProcedureCallStmt(const std::string& proc_name, 
			std::vector<std::string> args)
		: name(proc_name), arguments(std::move(args)) {}
	void accept(ASTVisitor& visitor) override;
};

class ReadStmt : public Statement{
public:
	std::unique_ptr<Expression> variable;
	ReadStmt(std::unique_ptr<Expression> var) : variable(std::move(var)) {}
	void accept(ASTVisitor& visitor) override;
};

class WriteStmt : public Statement{
public:
	std::unique_ptr<Expression>value;
	WriteStmt(std::unique_ptr<Expression> val) : value(std::move(val)) {}
	void accept(ASTVisitor& visitor) override;
};




//procedures and main program
class ProcedureDecl : public ASTNode{
public:
	std::unique_ptr<ProcedureHeadExpr> head;
	std::vector<std::unique_ptr<Expression>> parameters;
	std::vector<std::unique_ptr<Statement>> body;
	ProcedureDecl(std::unique_ptr<ProcedureHeadExpr> head,
			std::vector<std::unique_ptr<Expression>> params,
			std::vector<std::unique_ptr<Statement>> b)
		: head(std::move(head)), parameters(std::move(params)), body(std::move(b)) {}
	void accept(ASTVisitor& visitor) override;
};

class Main : public ASTNode {};

class MainProcedure : public Main{
public:
	std::vector<std::unique_ptr<Expression>> declarations;
	std::vector<std::unique_ptr<Statement>> body;
	MainProcedure(std::vector<std::unique_ptr<Expression>> decl,
			std::vector<std::unique_ptr<Statement>> b)
		: declarations(std::move(decl)), body(std::move(b)) {}
	void accept(ASTVisitor& visitor) override;
};

class MainProcedureNoDecl : public Main{
public:
	std::vector<std::unique_ptr<Statement>> body;
	MainProcedureNoDecl(std::vector<std::unique_ptr<Statement>> b)
		: body(std::move(b)) {}
	void accept(ASTVisitor& visitor) override;
};

class Program : public ASTNode{
public:
	std::vector<std::unique_ptr<ProcedureDecl>> procedures;
	std::unique_ptr<Main> main;
	Program(std::vector<std::unique_ptr<ProcedureDecl>> procs, 
		std::unique_ptr<Main> main)
		: procedures(std::move(procs)), main(std::move(main)) {}
	void accept(ASTVisitor& visitor) override;
};


#endif //AST_HPP
