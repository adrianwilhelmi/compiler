//Author: Adrian Wilhelmi

#ifndef AST_CODE_GENERATOR_HPP
#define AST_CODE_GENERATOR_HPP

#include<vector>
#include<string>
#include<unordered_map>
#include<sstream>
#include<fstream>

#include"ast.hpp"
#include"ast_visitor.hpp"

#include<cln/cln.h>

struct ArrayInfo{
	std::size_t base_addr;
	std::size_t from;
	std::size_t to;
};

class ASTCodeGenerator : ASTVisitor{
public:
	ASTCodeGenerator(std::ostream& output)
		: output_stream(output) {
		this->var_map["0"] = 0;
		this->next_free_addr = 1;
	}

	void visit(NumberExpr& expr) override{
		emit("SET" + std::to_string(expr.value));
	}

	void visit(VariableExpr& expr) override{
		auto it = var_map.find(expr.name);
		if(it == var_map.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.name);
		}

		emit(" access element " + expr.name)
		emit("LOAD " + std::to_string(it->second));
	}

	void visit(ArrayAccessWithIdExpr& expr) override{
		auto it = array_info.find(expr.array_name);
		if(it == array_info.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.array_name);
		}

		auto iit = var_map.find(expr.index);
		if(iit == var_map.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.index);
		}

		const auto&info = it->second;
		
		emit("# access array elemetn " + expr.array_name 
			+ "[" + expr.index + "]");
		emit("LOAD " + std::to_string(iit->second));
		emit("SUB " + std::to_string(info.from));
		emit("ADD " + std::to_string(info.base_addr));
		emit("LOADI 0");
	}

	void visit(ArrayAccessWithNumExpr& expr) override{
		auto it = array_info.find(expr.array_name);
		if(it == array_info.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.array_name);
		}

		const auto& info = it->second
		int64_t adjusted_index = expr.index - info.from;
		std::size_t real_index = info.base_address + adjusted_index;

		emit("#access array element " + expr.array_name + "[" 
			+ std::to_string(expr.index) + "]");
		emit("LOAD " + std::to_string(real_index));
	}

	void visit(ArrayDeclarationExpr& expr) override{
		if(expr.from > expr.to){
			throw std::runtime_error("invalid arr declaration" + expr.var + 
						" [" + std::to_string(expr.from) +
						":" + std::to_string(expr.to) "]");
		}

		std::size_t size = expr.to - expr.from + 1;
		std::size_t base_address = alloc_mem(expr.var, size);
		array_info[expr.var] = {base_address, expr.from, expr.to};
	}

	void visit(VariableDeclarationExpr& expr) override{
		alloc_mem(expr.var);
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
		expr.right->visit(*this);

		std::size_t temp_addr = alloc_temp_memory();
		emit("STORE " + std::to_string(temp_addr));

		expr.left->visit(*this);

		if(expr.op == "+"){
			emit("ADD " + std::to_string(temp_addr));
		}
		else if(expr.op == "-"){
			emit("SUB " + std::to_string(temp_addr));
		}
		else if(expr.op == "*"){
			// mul
		}
		else if(expr.op == "/"){
			// div
		}
		else if(expr.op == "%"){
			// mod
		}
		else{
			throw std::runtime_error("unsupported operator");
		}

		free_temp_memory();
	}

	void visit(ConditionExpr& expr) override{
		expr.left->visit(*this);
		expr.right->visit(*this);

		cln::cl_I left_res = expr.left.get_result();
		cln::cl_I right_res = expr.right.get_result();

		bool result;
		if(expr.op == "="){
			result = (left_res == right_res);
		}
		else if(expr.op == "!="){
			result = (left_res != right_res);
		}
		else if(expr.op == ">"){
			result = (left_res > right_res);
		}
		else if(expr.op == "<"){
			result = (left_res < right_res);
		}
		else if(expr.op == ">="){
			result = (left_res >= right_res);
		}
		else if(expr.op == "<="){
			result = (left_res <= right_res);
		}
		else{
			throw std::runtime_error("unsupported operator");
		}

		expr.set_result(result);
	}

	void visit(AssignStmt& stmt) override{
		//Expression* expr = stmt.
		//TUTAJ
		stmt.value->accept(*this);

		emit("STORE " + std::to_string(addr));
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
		Expression*expr = stmt.variable.get();
		
		
		else if(auto num_expr = dynamic_cast<NumberExpr*>(expr)){
			//
		}
		if(auto var_expr = dynamic_cast<VariableExpr*>(expr)){
			//
		}
		else if(auto arr_id_expr = dynamic_cast<ArrayAccessWithIdExpr*>(expr)){
			//
		}
		else if(auto arr_num_expr = dynamic_cast<ArrayAccessWithNumExpr*>(expr)){
			//
		}
		else{
			throw std::runtime_error("unknown expression type");
		}
		//odczytac wartosc z stmt.variable
	}

	void visit(WriteStmt& stmt) override{
		//
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
	}

	std::size_t alloc_mem(std::string name, std::size_t arr_size = 0){
		/*
		args:
			variable/array name, array_size if array
		returns:
			allocated memory address (1st addr if array) 
		exception:
			if variable already exists
		*/

		if(var_map.find(name) != var_map.end())
			throw std::runtime_error("variable name collision: " + name);
		var_map[name] = next_free_addr;

		if(arr_size == 0){
			this->next_free_addr++;
			return this->next_free_addr - 1;
		}
		this->next_free_addr += arr_size;
		return this->next_free_addr - arr_size;
	}

private:
	std::ostream& output_stream;				//output file
	std::size_t k;						//instr counter
	std::size_t next_free_addr;
	
	std::unordered_map<std::string, std::size_t> var_map;	//map var/arr -> mem addr
	std::unordered_map<std::string, ArrayInfo> arr_info;

	void emit(const std::string instruction){
		k++;
		output_stream << instruction;
	}
};

#endif // AST_CODE_GENERATOR_HPP
