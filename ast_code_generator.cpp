#ifndef AST_CODE_GENERATOR_HPP
#define AST_CODE_GENERATOR_HPP

#include"ast.hpp"
#include"ast_visitor.hpp"

class ASTCodeGenerator : ASTVisitor{
public:
	ASTCodeGenerator(std::ostream& output)
		: output_stream(output) {}

	std::string get_code() const{
		return generated_code.str();
	}

	void visit(NumberExpr& expr) override{
		//
	}
	void visit(VariableExpr& expr) override{
		//
	}
	void visit(ArrayAccessWithIdExpr& expr) override{
		//
	}
	void visit(ArrayAccessWithNumExpr& expr) override{
		//
	}
	void visit(ArrayDeclarationExpr& expr) override{
		//
	}
	void visit(VariableDeclarationExpr& expr) override{
		//
	}
	void visit(VariableArgDeclExpr& expr) override{
		//
	}
	void visit(ArrayArgDeclExpr& expr) override{
		//
	}
	void visit(ProcedureHeadExpr& expr) override{
		//
	}
	void visit(BinaryOpExpr& expr) override{
		
		//
	}
	void visit(ConditionExpr& expr) override{
		//
	}

	void visit(AssignStmt& stmt) override{
		
	}
	void visit(WhileStmt& stmt) override{
		//
	}
	void visit(ForStmt& stmt) override{
		
	}
	void visit(RepeatUntilStmt& stmt) override{
		//
	}
	void visit(IfStmt& stmt) override{
		//
	}
	void visit(ProcedureCallStmt& stmt) override{
		//
	}

	void visit(ReadStmt& stmt) override{
		//
	}

	void visit(WriteStmt& stmt) override{
		output_stream << "PUT ";
		stmt.value->accept(*this);
	}

	void visit(ProcedureDecl& procedure) override{
		//
	}

	void visit(MainProcedure& main) override{
		for(auto& decl : main.declarations){
			decl->accept(*this):
		}

		for(auto& stmt : main.body){
			stmt->accept(*this);
		}
	}

	void visit(MainProcedureNoDecl& main) override{
		for(auto& stmt : main.body){
			stmt->accept(*this);
		}
	}

	void visit(Program& program) override{
		for(auto& proc : program.procedures){
			proc->accept(*this);
		}
		program.main->accept(*this);

		output_stream << generated_code.str();
	}

private:
	std::ostringstream generated_code;
	std::ostream& output_stream;
	int64_t k;
	
	std::unordered_map<std::string, int64_t> var_map
};

#endif // AST_CODE_GENERATOR_HPP
