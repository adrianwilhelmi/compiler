//Author: Adrian Wilhelmi

#ifndef AST_CODE_GENERATOR_HPP
#define AST_CODE_GENERATOR_HPP

#include<vector>
#include<string>
#include<unordered_map>
#include<sstream>
#include<fstream>
#include<set>

#include"ast.hpp"
#include"ast_visitor.hpp"

struct ArrayInfo{
	std::size_t base_addr;
	std::size_t from_addr;
	std::size_t to_addr;
};

struct ProcedureInfo{
	std::size_t proc_start;
	std::vector<bool> param_type; //false -> varaible. true -> arr
	std::vector<std::string> param_names;
	std::size_t rtrn_addr = 0;
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
		expr.set_num_instr(1);
	}

	void visit(VariableExpr& expr) override{
		auto it = var_map.find(expr.name);
		if(it == var_map.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.name);
		}

		emit("LOAD " + std::to_string(it->second)); 
		expr.set_num_instr(1);
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

		emit("SET " + std::to_string(info.base_addr));
		emit("SUB " + std::to_string(info.from_addr));
		emit("ADD " + std::to_string(iit->second));
		emit("LOADI 0");

		expr.set_num_instr(4);
	}

	void visit(ArrayAccessWithNumExpr& expr) override{
		auto it = arr_info.find(expr.array_name);
		if(it == arr_info.end()){
			throw std::runtime_error("undefined variable: " 
						+ expr.array_name);
		}

		const auto& info = it->second;

		emit("SET " + std::to_string((int64_t)info.base_addr + expr.index));
		emit("SUB " + std::to_string(info.from_addr));
		emit("LOADI 0");

		expr.set_num_instr(3);
	}

	void visit(ArrayDeclarationExpr& expr) override{
		if(expr.from > expr.to){
			throw std::runtime_error("invalid arr declaration" + expr.var + " [" + std::to_string(expr.from) + ":" + std::to_string(expr.to) + "]");
		}

		std::size_t from_addr = alloc_mem("1_" + expr.var + "_from");
		std::size_t to_addr = alloc_mem("1_" + expr.var + "_to");

		emit("SET " + std::to_string(expr.from));
		emit("STORE " + std::to_string(from_addr));

		emit("SET " + std::to_string(expr.to));
		emit("STORE " + std::to_string(to_addr));

		std::size_t size = expr.to - expr.from + 1;
		std::size_t base_address = alloc_mem(expr.var, size);
		arr_info[expr.var] = {base_address, from_addr, to_addr};

		expr.set_num_instr(4);
	}

	void visit(VariableDeclarationExpr& expr) override{
		alloc_mem(expr.var);
		expr.set_num_instr(0);
	}

	void visit(VariableArgDeclExpr& expr) override{
		std::cout << "alokuje: " << std::endl;
		std::cout << expr.var << std::endl;

		alloc_mem(expr.var);

		expr.set_num_instr(0);
	}

	void visit(ArrayArgDeclExpr& expr) override{
		std::cout << "alokuje: " << std::endl;
		std::cout << expr.var << std::endl;
		std::cout << expr.var + "_from" << std::endl;
		std::cout << expr.var + "_to" << std::endl;

		std::size_t base_addr = alloc_mem(expr.var);
		std::size_t from_addr = alloc_mem(expr.var + "_from");
		std::size_t to_addr = alloc_mem(expr.var + "_to");

		arr_info[expr.var] = {base_addr, from_addr, to_addr};
			
		expr.set_num_instr(0);
	}

	void visit(ProcedureHeadExpr& expr) override{
		if(procedures.find(expr.proc_name) != procedures.end()){
			throw std::runtime_error("procedure name collision: " 
						+ expr.proc_name);
		}

		std::size_t proc_start = this->instructions.size();

		std::vector<bool> param_types;
		std::vector<std::string> param_names;

		for(auto& decl : expr.vars){
			decl->accept(*this);
			if(auto var_arg = dynamic_cast<VariableArgDeclExpr*>(decl.get())){
				param_types.push_back(false);
				param_names.push_back(var_arg->var);
			}
			else if(auto var_arg = dynamic_cast<ArrayArgDeclExpr*>(decl.get())){
				param_types.push_back(true);
				param_names.push_back(var_arg->var);
			}
		}

		std::size_t ret_addr = alloc_mem("00_" + expr.proc_name + "_returnaddr");

		this->procedures[expr.proc_name] = {proc_start, param_types, 
						param_names, ret_addr};
	}

	void visit(BinaryOpExpr& expr) override{
		expr.right->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		if(expr.op == "+"){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("ADD " + std::to_string(temp_addr));

			expr.set_num_instr(2 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "-"){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));

			expr.set_num_instr(2 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "*"){
			std::string temp_lsign = alloc_temp_memory();
			std::size_t lsign_addr = var_map[temp_lsign];

			std::string temp_rsign = alloc_temp_memory();
			std::size_t rsign_addr = var_map[temp_rsign];

			emit("STORE " + std::to_string(temp_addr));

			//sprawdz znak b
			emit("JPOS 7");
			emit("SUB " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp_addr));
			emit("STORE " + std::to_string(temp_addr));
			emit("SET 1");
			emit("STORE " + std::to_string(rsign_addr));
			emit("JUMP 3");

			emit("SET 0");
			emit("STORE " + std::to_string(rsign_addr));

			expr.left->accept(*this);

			std::string temp2 = alloc_temp_memory();
			std::size_t temp2_addr = var_map[temp2];
			emit("STORE " + std::to_string(temp2_addr));

			emit("JPOS 7");
			emit("SUB " + std::to_string(temp2_addr));
			emit("SUB " + std::to_string(temp2_addr));
			emit("STORE " + std::to_string(temp2_addr));
			emit("SET 1");
			emit("STORE " + std::to_string(lsign_addr));
			emit("JUMP 3");

			emit("SET 0");
			emit("STORE " + std::to_string(lsign_addr));

			std::string temp3 = alloc_temp_memory();
			std::size_t temp3_addr = var_map[temp3];

			std::string temp4 = alloc_temp_memory();
			std::size_t temp4_addr = var_map[temp4];

			emit("LOAD " + std::to_string(temp2_addr));
			emit("STORE " + std::to_string(temp4_addr));	//temp
			emit("SET 0");
			emit("STORE " + std::to_string(temp3_addr));	//result = 0

			//petla mnozenia
			//sprawdz najmniej znaczacy bit prawego operanda
			emit("LOAD " + std::to_string(temp2_addr));
			emit("JZERO 16");	//przejdz na koniec
			emit("HALF");
			emit("STORE " + std::to_string(temp4_addr));
			emit("LOAD " + std::to_string(temp2_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("JZERO 4"); // najmlodszy = 0 -> nie dodawaj a do wyniku

			// najmlodszy = 1 -> dodaj a do wyniku
			emit("LOAD " + std::to_string(temp3_addr));
			emit("ADD " + std::to_string(temp_addr));
			emit("STORE " + std::to_string(temp3_addr));

			// a = a * 2, b = b // 2
			emit("LOAD " + std::to_string(temp_addr));
			emit("ADD " + std::to_string(temp_addr));
			emit("STORE " + std::to_string(temp_addr));

			emit("LOAD " + std::to_string(temp4_addr));
			emit("STORE" + std::to_string(temp2_addr));
			emit("JUMP -15");

			emit("LOAD " + std::to_string(lsign_addr));
			emit("SUB " + std::to_string(rsign_addr));
			emit("JZERO 5");

			emit("LOAD " + std::to_string(temp3_addr));
			emit("SUB " + std::to_string(temp3_addr));
			emit("SUB " + std::to_string(temp3_addr));
			emit("JUMP 2");

			emit("LOAD " + std::to_string(temp3_addr));

			free_temp_memory(temp2);
			free_temp_memory(temp3);
			free_temp_memory(temp4);
			free_temp_memory(temp_lsign);
			free_temp_memory(temp_rsign);

			expr.set_num_instr(49 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "/"){
			std::string temp4 = alloc_temp_memory();
			std::size_t temp4_addr = var_map[temp4];

			std::string temp_lsign = alloc_temp_memory();
			std::size_t lsign_addr = var_map[temp_lsign];

			std::string temp_rsign = alloc_temp_memory();
			std::size_t rsign_addr = var_map[temp_rsign];

			emit("JZERO 57"); /* idz na koniec */
			emit("STORE " + std::to_string(temp4_addr));

			// zapamietaj znak prawej stroyn
			emit("JPOS 7");
			emit("SUB " + std::to_string(temp4_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("STORE " + std::to_string(temp4_addr));

			emit("SET 1");
			emit("STORE" + std::to_string(rsign_addr));

			emit("JUMP 3");

			emit("SET 0");
			emit("STORE" + std::to_string(rsign_addr));


			expr.left->accept(*this);
			emit("JZERO 45"); /* idz na koniec */
			emit("STORE " + std::to_string(temp_addr));

			emit("JPOS 7");
			emit("SUB " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp_addr));
			emit("STORE " + std::to_string(temp_addr));

			emit("SET 1");
			emit("STORE" + std::to_string(lsign_addr));

			emit("JUMP 3");

			emit("SET 0");
			emit("STORE" + std::to_string(lsign_addr));

			// zapamietaj znak prawej stroyn

			std::string temp3 = alloc_temp_memory();
			std::size_t temp3_addr = var_map[temp3];

			std::string temp2 = alloc_temp_memory();
			std::size_t temp2_addr = var_map[temp2];

			emit("SET 0");
			emit("STORE " + std::to_string(temp2_addr));	//result = 0
			emit("SET 1");
			emit("STORE " + std::to_string(temp3_addr));	//result = 0

			// znajdz najwieksza potege 2^k, t ze b2^k <= a
			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("JNEG 8"); /* przejdz do obliczania wyniku */
			emit("LOAD " + std::to_string(temp3_addr));
			emit("ADD " + std::to_string(temp3_addr));
			emit("STORE " + std::to_string(temp3_addr));
			emit("LOAD " + std::to_string(temp4_addr));
			emit("ADD " + std::to_string(temp4_addr));
			emit("STORE " + std::to_string(temp4_addr));
			emit("JUMP -9");
			
			emit("LOAD " + std::to_string(temp4_addr));

			// petla dzielenia
			emit("HALF");
			emit("STORE " + std::to_string(temp4_addr));

			emit("LOAD " + std::to_string(temp3_addr));
			emit("HALF");
			emit("STORE " + std::to_string(temp3_addr));

			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("JNEG 5"); // a < b -> przejdz do nastepnej iteracji
			emit("STORE " + std::to_string(temp_addr));

			emit("LOAD " + std::to_string(temp2_addr));
			emit("ADD " + std::to_string(temp3_addr));
			emit("STORE " + std::to_string(temp2_addr));

			emit("LOAD " + std::to_string(temp4_addr));
			emit("JPOS -13");

			emit("LOAD " + std::to_string(lsign_addr));
			emit("SUB " + std::to_string(rsign_addr));
			emit("JZERO 4");

			emit("SET -1");
			emit("SUB " + std::to_string(temp2_addr));
			emit("JUMP 2");

			emit("LOAD " + std::to_string(temp2_addr));

			free_temp_memory(temp2);
			free_temp_memory(temp3);
			free_temp_memory(temp4);
			free_temp_memory(temp_lsign);
			free_temp_memory(temp_rsign);

			expr.set_num_instr(58 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "%"){
			std::string temp4 = alloc_temp_memory();
			std::size_t temp4_addr = var_map[temp4];

			std::string temp_lsign = alloc_temp_memory();
			std::size_t lsign_addr = var_map[temp_lsign];

			std::string temp_rsign = alloc_temp_memory();
			std::size_t rsign_addr = var_map[temp_rsign];

			emit("JZERO 57"); /* idz na koniec */
			emit("STORE " + std::to_string(temp4_addr));

			// zapamietaj znak prawej stroyn
			emit("JPOS 7");
			emit("SUB " + std::to_string(temp4_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("STORE " + std::to_string(temp4_addr));

			emit("SET 1");
			emit("STORE" + std::to_string(rsign_addr));

			emit("JUMP 3");

			emit("SET 0");
			emit("STORE" + std::to_string(rsign_addr));


			expr.left->accept(*this);
			emit("JZERO 45"); /* idz na koniec */
			emit("STORE " + std::to_string(temp_addr));

			emit("JPOS 7");
			emit("SUB " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp_addr));
			emit("STORE " + std::to_string(temp_addr));

			emit("SET 1");
			emit("STORE" + std::to_string(lsign_addr));

			emit("JUMP 3");

			emit("SET 0");
			emit("STORE" + std::to_string(lsign_addr));


			// zapamietaj znak prawej stroyn

			std::string temp3 = alloc_temp_memory();
			std::size_t temp3_addr = var_map[temp3];

			std::string temp2 = alloc_temp_memory();
			std::size_t temp2_addr = var_map[temp2];

			emit("SET 0");
			emit("STORE " + std::to_string(temp2_addr));	//result = 0
			emit("LOAD " + std::to_string(temp4_addr));
			emit("STORE " + std::to_string(temp3_addr));	//result = 0

			// znajdz najwieksza potege 2^k, t ze b2^k <= a
			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp3_addr));
			emit("JNEG 5"); /* przejdz do obliczania wyniku */
			emit("LOAD " + std::to_string(temp3_addr));
			emit("ADD " + std::to_string(temp3_addr));
			emit("STORE " + std::to_string(temp3_addr));
			emit("JUMP -6");
			
			emit("LOAD " + std::to_string(temp3_addr));

			// petla dzielenia
			emit("HALF");
			emit("STORE " + std::to_string(temp3_addr));

			emit("SUB " + std::to_string(temp4_addr));
			emit("JNEG 5"); // a < b -> przejdz do nastepnej iteracji

			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp3_addr));
			emit("JNEG 2");
			emit("STORE " + std::to_string(temp_addr));

			emit("LOAD " + std::to_string(temp3_addr));
			emit("JPOS -9");

			emit("LOAD " + std::to_string(lsign_addr));
			emit("SUB " + std::to_string(rsign_addr));
			emit("JZERO 4");

			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(temp4_addr));
			emit("JUMP 2");

			emit("LOAD " + std::to_string(temp_addr));

			free_temp_memory(temp2);
			free_temp_memory(temp3);
			free_temp_memory(temp4);
			free_temp_memory(temp_lsign);
			free_temp_memory(temp_rsign);

			expr.set_num_instr(51 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else{
			throw std::runtime_error("unsupported operator");
		}

		free_temp_memory(temp);
	}

	void visit(ConditionExpr& expr) override{
		// (acc == 0) -> TRUE. w przeciwnym -> FALSE
		expr.right->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		if(expr.op == "="){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));

			expr.set_num_instr(2 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "!="){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));
			emit("JZERO 3");
			emit("SUB 0");
			emit("JUMP 2");
			emit("SET 1");

			expr.set_num_instr(6 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == ">"){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));
			emit("JPOS 3");
			emit("SET 1");
			emit("JUMP 2");
			emit("SUB 0");

			expr.set_num_instr(6 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "<"){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));
			emit("JNEG 3");
			emit("SET 1");
			emit("JUMP 2");
			emit("SUB 0");

			expr.set_num_instr(6 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == ">="){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));
			emit("JNEG 2");
			emit("SET 0");

			expr.set_num_instr(4 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else if(expr.op == "<="){
			emit("STORE " + std::to_string(temp_addr));
			expr.left->accept(*this);
			emit("SUB " + std::to_string(temp_addr));
			emit("JPOS 2");
			emit("SET 0");

			expr.set_num_instr(4 + expr.left->get_num_instr() 
					+ expr.right->get_num_instr());
		}
		else{
			throw std::runtime_error("unsupported operator");
		}

		free_temp_memory(temp);
	}

	void visit(AssignStmt& stmt) override{
		//stmt.variable->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		std::size_t num_instr = 0;

		if(auto expr = dynamic_cast<VariableExpr*>(stmt.variable.get())){
			auto it = var_map.find(expr->name);
			if(it == var_map.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->name);
			}

			if(loop_iterators.find(expr->name) != loop_iterators.end()){
				throw std::runtime_error("loop iterator '" + expr->name + "' modification");
			}

			emit("SET " + std::to_string(it->second));

			num_instr = 1;
		}
		else if(auto expr = dynamic_cast<ArrayAccessWithIdExpr*>(stmt.variable.get())){
			auto it = arr_info.find(expr->array_name);
			if(it == arr_info.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->array_name);
			}

			if(loop_iterators.find(expr->array_name) != loop_iterators.end()){
				throw std::runtime_error("loop iterator '" + expr->array_name + "' modification");
			}

			auto iit = var_map.find(expr->index);
			if(iit == var_map.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->index);
			}

			const auto&info = it->second;

			//std::size_t from_addr = var_map["0_" + expr->array_name + "_from"];

			emit("SET " + std::to_string(info.base_addr));
			emit("SUB " + std::to_string(info.from_addr));
			emit("ADD " + std::to_string(iit->second));

			num_instr = 3;
		}
		else if(auto expr = dynamic_cast<ArrayAccessWithNumExpr*>(stmt.variable.get())){
			auto it = arr_info.find(expr->array_name);
			if(it == arr_info.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->array_name);
			}

			if(loop_iterators.find(expr->array_name) != loop_iterators.end()){
				throw std::runtime_error("loop iterator '" + expr->array_name + "' modification");
			}

			const auto& info = it->second;

			//std::size_t from_addr = var_map["0_" + expr->array_name + "_from"];

			emit("SET " + std::to_string((int64_t)info.base_addr + expr->index));
			emit("SUB " + std::to_string(info.from_addr));

			num_instr = 2;
		}
		else{
			throw std::runtime_error("Assign: unknown type");
		}

		emit("STORE " + std::to_string(temp_addr));
		stmt.value->accept(*this);
		emit("STOREI " + std::to_string(temp_addr));
		free_temp_memory(temp);

		stmt.set_num_instr(2 + num_instr + stmt.value->get_num_instr());
	}

	void visit(WhileStmt& stmt) override{
		stmt.condition->accept(*this);

		std::size_t body_start = this->instructions.size();

		emit("JPOS ?");
		emit("JNEG ?");

		std::size_t body_num_instr = 0;

		for(auto&command : stmt.body){
			command->accept(*this);
			body_num_instr += command->get_num_instr();
		}

		std::size_t num_instr = stmt.condition->get_num_instr();
		num_instr += body_num_instr;

		emit("JUMP -" + std::to_string(num_instr + 1));
		emit("JPOS " + std::to_string(body_num_instr + 2), body_start);
		emit("JNEG " + std::to_string(body_num_instr + 1), body_start + 1);

		num_instr += 3;
		stmt.set_num_instr(num_instr);
	}

	void visit(ForStmt& stmt) override{
		std::size_t num_instr = 0;

		alloc_mem(stmt.iterator);
		std::size_t iter_addr = var_map[stmt.iterator];

		loop_iterators.insert(stmt.iterator);

		stmt.from->accept(*this);
		num_instr += stmt.from->get_num_instr();
		emit("STORE " + std::to_string(iter_addr));

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		stmt.to->accept(*this);
		num_instr += stmt.to->get_num_instr();
		emit("STORE " + std::to_string(temp_addr));

		std::string temp_one = alloc_temp_memory();
		std::size_t one_addr = var_map[temp_one];
		emit("SET 1");
		emit("STORE " + std::to_string(one_addr));

		if(stmt.downto){
			emit("LOAD " + std::to_string(iter_addr));
			emit("SUB " + std::to_string(temp_addr));
			emit("JNEG ?");
		}
		else{
			emit("LOAD " + std::to_string(temp_addr));
			emit("SUB " + std::to_string(iter_addr));
			emit("JNEG ?");
		}

		std::size_t jzero_pos = instructions.size() - 1;

		num_instr += 7;

		std::size_t body_num_instr = 0;

		for(auto& command : stmt.body){
			command->accept(*this);
			body_num_instr += command->get_num_instr();
		}

		num_instr += body_num_instr;

		if(stmt.downto){
			emit("LOAD " + std::to_string(iter_addr));
			emit("SUB " + std::to_string(one_addr));
			emit("STORE " + std::to_string(iter_addr));
		}
		else{
			emit("LOAD " + std::to_string(iter_addr));
			emit("ADD " + std::to_string(one_addr));
			emit("STORE " + std::to_string(iter_addr));
		}

		num_instr += 3;

		emit("JUMP -" + std::to_string(body_num_instr + 6));
		emit("JNEG " + std::to_string(body_num_instr + 5),
				jzero_pos);

		num_instr += 1;

		loop_iterators.erase(stmt.iterator);
		var_map.erase(stmt.iterator);

		free_temp_memory(temp);
		free_temp_memory(temp_one);

		stmt.set_num_instr(num_instr);
	}

	void visit(RepeatUntilStmt& stmt) override{
		std::size_t num_instr = 0;
		std::size_t body_num_instr = 0;
		for(auto& command : stmt.body){
			command->accept(*this);
			body_num_instr += command->get_num_instr();
		}

		stmt.condition->accept(*this);
		num_instr = stmt.condition->get_num_instr() + body_num_instr;

		emit("JNEG -" + std::to_string(num_instr));
		emit("JPOS -" + std::to_string(num_instr + 1));

		stmt.set_num_instr(num_instr + 2);
	}

	void visit(IfStmt& stmt) override{
		stmt.condition->accept(*this);

		std::size_t then_start = this->instructions.size();

		emit("JPOS ?"); //ile przeskoczyc?
		emit("JNEG ?"); //ile przeskoczyc?

		std::size_t num_instr = 2;
		std::size_t then_num_instr = 0;

		for(auto& command : stmt.then_body){
			command->accept(*this);
			then_num_instr += command->get_num_instr();
		}

		emit("JPOS " + std::to_string(then_num_instr + 3), 
			then_start);
		emit("JNEG " + std::to_string(then_num_instr + 2),
			then_start + 1);

		num_instr += then_num_instr;

		if(!stmt.else_body.empty()){
			std::size_t else_start = this->instructions.size();
			emit("JUMP ?");

			std::size_t else_num_instr = 0;
			for(auto& command : stmt.else_body){
				command->accept(*this);
				else_num_instr += command->get_num_instr();
			}

			emit("JUMP " + std::to_string(else_num_instr + 1),
				else_start);

			num_instr += else_num_instr + 1;
		}

		num_instr += stmt.condition->get_num_instr();
		stmt.set_num_instr(num_instr);
	}

	void visit(ProcedureCallStmt& stmt) override{
		auto& proc_info = procedures[stmt.name];

		if(stmt.arguments.size() != proc_info.param_type.size()){
			throw std::runtime_error("wrong number of arguments to procedure: ");
		}

		std::size_t num_instr = 0;

		std::size_t ctr = 0;
		for(auto& arg : stmt.arguments){
			if(arr_info.find(arg) != arr_info.end()){
				if(proc_info.param_type[ctr] == false){
					throw std::runtime_error(
						std::to_string(ctr) + 
						" argument: '" + 
						arg + "' of procedure " +
						stmt.name + " should be a variable");
				}

				auto& arg_info = arr_info[arg];
				auto& info = arr_info[proc_info.param_names[ctr]];

				emit("LOAD " + std::to_string(arg_info.base_addr));
				emit("STORE " + std::to_string(info.base_addr));

				emit("LOAD " + std::to_string(arg_info.from_addr));
				emit("STORE " + std::to_string(info.from_addr));

				emit("LOAD " + std::to_string(arg_info.to_addr));
				emit("STORE " + std::to_string(info.to_addr));

				num_instr += 6;
			}
			else if(var_map.find(arg) != var_map.end()){
				if(proc_info.param_type[ctr] == true){
					throw std::runtime_error(
						std::to_string(ctr) + 
						" argument: '" + 
						arg + "' of procedure " +
						stmt.name + " should be an array");
				}

				std::size_t arg_addr = var_map[arg];
				std::size_t addr = var_map[proc_info.param_names[ctr]];

				std::cout << "addr " << addr << std::endl;

				emit("LOAD " + std::to_string(arg_addr));
				emit("STORE " + std::to_string(addr));

				num_instr += 2;
			}
			else{
				throw std::runtime_error("undeclared variable: '" +
						arg + "'");
			}

			ctr++;
		}

		emit("SET " + std::to_string(this->instructions.size() + 5));
		emit("STORE " + std::to_string(proc_info.rtrn_addr));
		emit("SET " + std::to_string(proc_info.proc_start));
		emit("RTRN 0");

		num_instr += 4;

		/*
		for(auto& arg : stmt.arguments){
			if(arr_info.find(arg) != arr_info.end()){
				auto& arg_info = arr_info[arg];
				auto& info = arr_info[proc_info.param_names[ctr]];

				emit("LOAD " + info.base_addr);
				emit("STORE " + arg_info.base_addr);

				emit("LOAD " + arg_info.from_addr);
				emit("STORE " + info.from_addr);

				emit("LOAD " + arg_info.to_addr);
				emit("STORE " + std::to_string(info.to_addr));

				num_instr += 6;
			}
			else{
				std::size_t arg_addr = var_map[arg];
				std::size_t addr = var_map[proc_info.param_names[ctr]];

				std::cout << "addr " << addr << std::endl;

				emit("LOAD " + std::to_string(arg_addr));
				emit("STORE " + std::to_string(addr));

				num_instr += 2;
			}

			ctr++;
		}
		*/

		stmt.set_num_instr(num_instr);
	}

	void visit(ReadStmt& stmt) override{
		//stmt.variable->accept(*this);

		std::string temp = alloc_temp_memory();
		std::size_t temp_addr = var_map[temp];

		std::size_t num_instr;

		if(auto expr = dynamic_cast<VariableExpr*>(stmt.variable.get())){
			auto it = var_map.find(expr->name);
			if(it == var_map.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->name);
			}

			emit("SET " + std::to_string(it->second));

			num_instr = 1;
		}
		else if(auto expr = dynamic_cast<ArrayAccessWithIdExpr*>(stmt.variable.get())){
			auto it = arr_info.find(expr->array_name);
			if(it == arr_info.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->array_name);
			}

			auto iit = var_map.find(expr->index);
			if(iit == var_map.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->index);
			}

			const auto&info = it->second;

			//std::size_t from_addr = var_map["0_" + expr->array_name + "_from"];

			emit("SET " + std::to_string(info.base_addr));
			emit("SUB " + std::to_string(info.from_addr));
			emit("ADD " + std::to_string(iit->second));

			num_instr = 3;
		}
		else if(auto expr = dynamic_cast<ArrayAccessWithNumExpr*>(stmt.variable.get())){
			auto it = arr_info.find(expr->array_name);
			if(it == arr_info.end()){
				throw std::runtime_error("undefined variable: " 
							+ expr->array_name);
			}

			const auto& info = it->second;

			//std::size_t from_addr = var_map["0_" + expr->array_name + "_from"];

			emit("SET " + std::to_string(info.base_addr + expr->index));
			emit("SUB " + std::to_string(info.from_addr));

			num_instr = 2;
		}
		else{
			throw std::runtime_error("Read: unknown type");
		}


		emit("STORE " + std::to_string(temp_addr));
		emit("GET 0");
		emit("STOREI " + std::to_string(temp_addr));

		free_temp_memory(temp);

		stmt.set_num_instr(3 + num_instr);
	}

	void visit(WriteStmt& stmt) override{
		stmt.value->accept(*this);
		emit("PUT 0");

		stmt.set_num_instr(1 + stmt.value->get_num_instr());
	}

	void visit(ProcedureDecl& procedure) override{
		std::size_t num_instr = 0;

		std::string proc_name = procedure.head->proc_name;
		procedure.head->preprocess(proc_name);

		procedure.head->accept(*this);
		num_instr += procedure.head->get_num_instr();

		for(auto& decl : procedure.parameters){
			decl->preprocess(proc_name);
		}

		for(auto& command : procedure.body){
			command->preprocess(proc_name);
		}

		for(auto& decl : procedure.parameters){
			decl->accept(*this);
			num_instr += decl->get_num_instr();
		}

		for(auto& command : procedure.body){
			command->accept(*this);
			num_instr += command->get_num_instr();
		}

		auto& pinfo = procedures[proc_name];
		std::size_t raddr = pinfo.rtrn_addr;
		emit("RTRN " + std::to_string(raddr));

		procedure.set_num_instr(1 + num_instr);
	}

	void visit(MainProcedure& main) override{
		std::size_t num_instr = 0;
		for(auto& decl : main.declarations){
			decl->accept(*this);
			num_instr += decl->get_num_instr();
		}

		for(auto& stmt : main.body){
			stmt->accept(*this);
			num_instr += stmt->get_num_instr();
		}
		main.set_num_instr(num_instr);
	}

	void visit(MainProcedureNoDecl& main) override{
		std::size_t num_instr = 0;
		for(auto& stmt : main.body){
			stmt->accept(*this);
			num_instr += stmt->get_num_instr();
		}
		main.set_num_instr(num_instr);
	}

	void visit(Program& program) override{
		std::size_t num_instr = 2;

		emit("JUMP ?");

		for(auto& proc : program.procedures){
			proc->accept(*this);
			num_instr += proc->get_num_instr();
		}
		emit("JUMP " + std::to_string(num_instr), 0);

		program.main->accept(*this);
		num_instr += program.main->get_num_instr();


		emit("HALT");
		program.set_num_instr(num_instr);

		std::cout << "num instructions: " << num_instr << std::endl;
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

	std::string alloc_temp_memory(std::size_t arr_size = 0){
		/*
		args:
			(optional) array size
		returns:
			allocated temporary variable's name
		*/

		std::string temp = "0_TEMPORARY" + std::to_string(next_free_addr);
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
		if(var_map.find(temp) == var_map.end()){
			return;
			//throw std::runtime_error("no such variable: " + temp);
		}

		var_map.erase(temp);

		/*
		if(arr_info.find(temp) != arr_info.end()){
			auto&info = arr_info[temp];
			next_free_addr -= info.to;
			arr_info.erase(temp);
			return;
		}
		*/
		next_free_addr--;
	}

	void push_code_to_file(){
		for(const auto& instr : this->instructions){
			output_stream << instr << "\n";
		}
	}

private:
	std::vector<std::string> instructions;
	std::ostream& output_stream;				//output file
	std::size_t next_free_addr;
	
	std::set<std::string> loop_iterators;

	std::unordered_map<std::string, std::size_t> var_map;	
	std::unordered_map<std::string, ArrayInfo> arr_info;
	std::unordered_map<std::string, ProcedureInfo> procedures;

	void emit(const std::string& instruction, int64_t pos = -1){
		if(pos == -1){
			instructions.push_back(instruction);
		}
		else{
			instructions[pos] = instruction;
		}
	}

	// SPRAWDZIC CZY ZMIENNA ZAINICJLIZIOWANA
};

#endif // AST_CODE_GENERATOR_HPP
