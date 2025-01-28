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

struct ArrayInfo{
	std::size_t base_addr;
	int64_t from;
	int64_t to;
};

class ASTCodeGenerator : public ASTVisitor{
public:
	ASTCodeGenerator(std::ostream& output)
		: output_stream(output) {
		this->var_map["0"] = 0;
		this->next_free_addr = 1;
	}

	void visit(NumberExpr& expr) override{
		emit("SET " + std::to_string(expr.value));
	}

	void visit(VariableExpr& expr) override{
		auto it = var_map.find(expr.name);
		if(it == var_map.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.name);
		}

		emit("# access element " + expr.name);
		//emit("LOAD " + std::to_string(it->second)); // wartosc do akumulatori
		emit("SET " + std::to_string(it->second)); // adres szukanego var 
	}

	void visit(ArrayAccessWithIdExpr& expr) override{
		auto it = arr_info.find(expr.array_name);
		if(it == arr_info.end()){
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
		//emit("LOADI 0"); // wartosc do akumulatora
		//ok
	}

	void visit(ArrayAccessWithNumExpr& expr) override{
		auto it = arr_info.find(expr.array_name);
		if(it == arr_info.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.array_name);
		}

		const auto& info = it->second;
		int64_t adjusted_index = expr.index - info.from;
		std::size_t real_index = info.base_addr + adjusted_index;

		emit("#access array element " + expr.array_name + "[" 
			+ std::to_string(expr.index) + "]");
		//emit("LOAD " + std::to_string(real_index)); // wartosc do akumulatora
		emit("SET " + std::to_string(real_index));
	}

	void visit(ArrayDeclarationExpr& expr) override{
		if(expr.from > expr.to){
			throw std::runtime_error("invalid arr declaration" + expr.var + " [" + std::to_string(expr.from) + ":" + std::to_string(expr.to) + "]");
		}

		std::size_t size = expr.to - expr.from + 1;
		std::size_t base_address = alloc_mem(expr.var, size);
		arr_info[expr.var] = {base_address, expr.from, expr.to};
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
		expr.right->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		emit("STORE " + std::to_string(temp_addr));

		expr.left->accept(*this);
		emit("LOADI 0");

		if(expr.op == "+"){
			emit("ADDI " + std::to_string(temp_addr));
		}
		else if(expr.op == "-"){
			emit("SUBI " + std::to_string(temp_addr));
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

		free_temp_memory(temp);
	}

	void visit(ConditionExpr& expr) override{
		expr.right->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];
		emit("STORE " + std::to_string(temp_addr));

		expr.left->accept(*this);

		if(expr.op == "="){
			//
		}
		else if(expr.op == "!="){
			//
		}
		else if(expr.op == ">"){
			//
		}
		else if(expr.op == "<"){
			//
		}
		else if(expr.op == ">="){
			//
		}
		else if(expr.op == "<="){
			//
		}
		else{
			throw std::runtime_error("unsupported operator");
		}

		free_temp_memory(temp);
	}

	void visit(AssignStmt& stmt) override{
		//Expression* expr = stmt.variable.get();
		//TUTAJ

		stmt.variable->accept(*this);
		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];
		emit("STORE " + std::to_string(temp_addr));

		stmt.value->accept(*this);
		emit("STOREI " + std::to_string(temp_addr));
		free_temp_memory(temp);
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
		stmt.variable->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];
		emit("STORE " + std::to_string(temp_addr));

		emit("GET 0");
		emit("STOREI " + std::to_string(temp_addr));

		free_temp_memory(temp);

		/*
		Expression*expr = stmt.variable.get();
		
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
		*/
	}

	void visit(WriteStmt& stmt) override{
		stmt.value->accept(*this);
		emit("LOADI 0");
		emit("PUT 0");
		//
	}

	void visit(ProcedureDecl& procedure) override{
		//
	}

	void visit(MainProcedure& main) override{
		for(auto& decl : main.declarations){
			decl->accept(*this);
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

		emit("HALT");
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

	std::string alloc_temp_memory(int64_t arr_size = 0){
		/*
		args:
			(optional) array size
		returns:
			allocated temporary variable's name
		*/

		std::string temp = "__TEMPORARY" + std::to_string(next_free_addr);
		var_map[temp] = next_free_addr;

		if(arr_size == 0){
			this->next_free_addr++;
			//return this->next_free_addr - 1;
			return temp;
		}
		arr_info[temp] = {next_free_addr, 0, arr_size};
		this->next_free_addr += arr_size;
		return temp;
	}

	void free_temp_memory(std::string temp){
		if(var_map.find(temp) == var_map.end())
			throw std::runtime_error("no such variable: " + temp);

		var_map.erase(temp);

		if(arr_info.find(temp) != arr_info.end()){
			auto&info = arr_info[temp];
			next_free_addr -= info.to;
			arr_info.erase(temp);
			return;
		}
		next_free_addr--;
	}

private:
	std::ostream& output_stream;				//output file
	std::size_t k;						//instr counter
	std::size_t next_free_addr;
	
	std::unordered_map<std::string, std::size_t> var_map;	//map var/arr -> mem addr
	std::unordered_map<std::string, ArrayInfo> arr_info;

	void emit(const std::string instruction){
		k++;
		output_stream << instruction << "\n";
	}
};

#endif // AST_CODE_GENERATOR_HPP
