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

class ArrayAccessExpr : public Expression {
public:
	std::string array_name;
	std::unique_ptr<Expression> index;
	ArrayAccessExpr(const std::string&name, std::unique_ptr<Expression> idx)
		: array_name(name), index(std::move(idx)) {}
	void accept(ASTVisitor& visitor) override;
};

class ArrayDeclarationExpr : public Expression {
public:
	std::unique_ptr<Expression> var;
	std::unique_ptr<Expression> from;
	std::unique_ptr<Expression> to;

	ArrayDeclarationExpr(std::unique_ptr<Expression> variable,
				std::unique_ptr<Expression> from,
				std::unique_ptr<Expression> to)
		: var(variable), from(from), to(to) {}
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
	EqCondition(const std::string&opr, std::unique_ptr<Expression> lhs,
			std::unique_ptr<Expression>rhs)
		: op(opr), left(std::move(lhs)), right(std::move(rhs)) {}
	void accept(ASTVisitor& visitor) override;
};




/*statements*/
class Statement : public ASTNode{};

class AssignStmt : public Statement{
public:
	std::string variable;
	std::unique_ptr<Expression> value;
	AssignStmt(const std::string& var, std::unique_ptr<Expression> val) 
		: variable(var), value(std::move(val)) {}
	void accept(ASTVisitor& visitor) override;
};

class WhileStmt : public Statement{
public:
	std::vector<std::unique_ptr<Statement>> body;
	std::unique_ptr<Expression> condition;
	WhileStmt(std::vector<std::unique_ptr<Statement>> b,
			std::unique_ptr<Expression> cond)
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
			std::vector<std::unique_ptr<Statement>> b)
			bool down,
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
	std::vector<std::unique_ptr<Expression>> arguments;
	ProcedureCallStmt(const std::string& proc_name, 
			std::vector<std::unique_ptr<Expression>> args)
		: name(proc_name), arguments(std::move(args)) {}
	void accept(ASTVisitor& visitor) override;
};

class ReadStmt : public Statement{
public:
	std::string variable;
	ReadStmt(const std::string& var) : variable(var) {}
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
	std::string name;
	std::vector<std::string> parameters;
	std::vector<std::unique_ptr<Statement>> body;
	ProcedureDecl(const std::string& proc_name, 
			std::vector<std::string> params,
			std::vector<std::unique_ptr<Statement>> b)
		: name(proc_name), parameters(std::move(params)), body(std::move(b)) {}
	void accept(ASTVisitor& visitor) override;
};

class Program : public ASTNode{
public:
	std::vector<std::unique_ptr<ProcedureDecl>> procedures;
	std::vector<std::unique_ptr<Statement>> main_body;
	Program(std::vector<std::unique_ptr<ProcedureDecl>> procs, 
		std::vector<std::unique_ptr<Statement>> body)
		: procedures(std::move(procs)), main_body(std::move(body)) {}
	void accept(ASTVisitor& visitor) override;
};


#endif //AST_HPP
