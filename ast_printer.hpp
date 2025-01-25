#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include<iostream>

#include"ast.hpp"
#include"ast_visitor.hpp"

// author: ChatGPT (prompt engineer: Adrian Wilhelmi)

// Klasa wizytora do debugowania i drukowania AST
struct ASTPrinter : ASTVisitor {
    int indentLevel = 0;

    void printIndent() {
        for (int i = 0; i < indentLevel; i++) std::cout << "  ";
    }

    void visit(NumberExpr& expr) override {
        printIndent();
        std::cout << "NumberExpr: " << expr.value << std::endl;
    }

    void visit(VariableExpr& expr) override {
        printIndent();
        std::cout << "VariableExpr: " << expr.name << std::endl;
    }

    void visit(ArrayAccessWithIdExpr& expr) override {
        printIndent();
        std::cout << "ArrayAccessExpr: " << expr.array_name << "[" << std::endl;
        indentLevel++;
        printIndent();
	std::cout << expr.index << std::endl;
        indentLevel--;
        printIndent();
        std::cout << "]" << std::endl;
    }

    void visit(ArrayAccessWithNumExpr& expr) override {
        printIndent();
        std::cout << "ArrayAccessExpr: " << expr.array_name << "[" << std::endl;
        indentLevel++;
        printIndent();
	std::cout << expr.index << std::endl;
        indentLevel--;
        printIndent();
        std::cout << "]" << std::endl;
    }


    void visit(BinaryOpExpr& expr) override {
        printIndent();
        std::cout << "BinaryOpExpr: " << expr.op << std::endl;
        indentLevel++;
        expr.left->accept(*this);
        expr.right->accept(*this);
        indentLevel--;
    }

    void visit(AssignStmt& stmt) override {
        printIndent();
        std::cout << "AssignStmt: " << std::endl;
        indentLevel++;
	stmt.variable->accept(*this);
        stmt.value->accept(*this);
        indentLevel--;
    }

    void visit(ForStmt& stmt) override {
        printIndent();
        std::cout << "ForStmt: " << stmt.iterator << " from" << std::endl;
        indentLevel++;
        stmt.from->accept(*this);
        printIndent();
        std::cout << (stmt.downto ? "downto" : "to") << std::endl;
        stmt.to->accept(*this);
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
        indentLevel--;
    }

    void visit(RepeatUntilStmt& stmt) override {
        printIndent();
        std::cout << "RepeatUntilStmt:" << std::endl;
        indentLevel++;
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
        printIndent();
        std::cout << "Until:" << std::endl;
        stmt.condition->accept(*this);
        indentLevel--;
    }

    void visit(IfStmt& stmt) override {
        printIndent();
        std::cout << "IfStmt:" << std::endl;
        indentLevel++;
        printIndent();
        std::cout << "Condition:" << std::endl;
        stmt.condition->accept(*this);
        printIndent();
        std::cout << "Then:" << std::endl;
        for (auto& s : stmt.then_body) {
            s->accept(*this);
        }
        if (!stmt.else_body.empty()) {
            printIndent();
            std::cout << "Else:" << std::endl;
            for (auto& s : stmt.else_body) {
                s->accept(*this);
            }
        }
        indentLevel--;
    }

    void visit(ProcedureCallStmt& stmt) override {
        printIndent();
        std::cout << "ProcedureCallStmt: " << stmt.name << " with arguments:" << std::endl;
        indentLevel++;
        for (auto& arg : stmt.arguments) {	
		std::cout << arg << ", ";
        }
	std::cout << std::endl;
        indentLevel--;
    }

    void visit(ReadStmt& stmt) override {
        printIndent();
	indentLevel++;
	stmt.variable->accept(*this);
	indentLevel--;
    }

    void visit(WriteStmt& stmt) override {
        printIndent();
        std::cout << "WriteStmt:" << std::endl;
        indentLevel++;
        stmt.value->accept(*this);
        indentLevel--;
    }

    void visit(ProcedureDecl& proc) override {
        printIndent();
        std::cout << "ProcedureDecl: " << std::endl;
        indentLevel++;
	proc.head->accept(*this);
        for (const auto& param : proc.parameters) {
            printIndent();
	    param->accept(*this);	
        }
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto& stmt : proc.body) {
            stmt->accept(*this);
        }
        indentLevel--;
    }

    void visit(Program& program) override {
        printIndent();
        std::cout << "Program:" << std::endl;
        indentLevel++;
        for (auto& proc : program.procedures) {
            proc->accept(*this);
        }
        printIndent();
	program.main->accept(*this);
        indentLevel--;
    }

	void visit(ArrayDeclarationExpr& expr) override {
	    printIndent();
	    std::cout << "ArrayDeclarationExpr:" << std::endl;
	    indentLevel++;
	    printIndent();
	    std::cout << "Variable: " << expr.var << std::endl;
	    printIndent();
	    std::cout << "From:" << expr.from << std::endl;
	    printIndent();
	    std::cout << "To:" << expr.to << std::endl;
	    indentLevel--;
	}

	void visit(VariableDeclarationExpr& expr) override {
	    printIndent();
	    std::cout << "VariableDeclarationExpr:" << std::endl;
	    indentLevel++;
	    printIndent();
	    std::cout << "Variable: " << expr.var;
	    printIndent();
	    indentLevel--;
	}


	void visit(ProcedureHeadExpr& expr) override {
	    printIndent();
	    std::cout << "ProcedureHeadExpr: " << std::endl;
	    indentLevel++;
		std::cout << expr.proc_name << std::endl;
	    printIndent();
	    std::cout << "Parameters:" << std::endl;
	    for (auto& var : expr.vars) {
		std::cout << var << ", ";
	    }
		std::cout << std::endl;
	    indentLevel--;
	}

	void visit(ConditionExpr& expr) override {
	    printIndent();
	    std::cout << "ConditionExpr: " << expr.op << std::endl;
	    indentLevel++;
	    expr.left->accept(*this);
	    expr.right->accept(*this);
	    indentLevel--;
	}

	void visit(WhileStmt& stmt) override {
	    printIndent();
	    std::cout << "WhileStmt:" << std::endl;
	    indentLevel++;
	    printIndent();
	    std::cout << "Condition:" << std::endl;
	    stmt.condition->accept(*this);
	    printIndent();
	    std::cout << "Body:" << std::endl;
	    for (auto& s : stmt.body) {
		s->accept(*this);
	    }
	    indentLevel--;
	}

	void visit(MainProcedure& mainProcedure) {
	    std::cout << "MainProcedure {" << std::endl;

	    // Drukuj deklaracje
	    std::cout << "  Declarations:" << std::endl;
	    for (const auto& decl : mainProcedure.declarations) {
		decl->accept(*this); // Odwiedź każdą deklarację
	    }

	    // Drukuj ciało
	    std::cout << "  Body:" << std::endl;
	    for (const auto& stmt : mainProcedure.body) {
		stmt->accept(*this); // Odwiedź każdą instrukcję
	    }

	    std::cout << "}" << std::endl;
	}

	void visit(MainProcedureNoDecl& mainProcedureNoDecl) {
	    std::cout << "MainProcedureNoDecl {" << std::endl;

	    // Drukuj ciało
	    std::cout << "  Body:" << std::endl;
	    for (const auto& stmt : mainProcedureNoDecl.body) {
		stmt->accept(*this); // Odwiedź każdą instrukcję
	    }

	    std::cout << "}" << std::endl;
	}
};


#endif //AST_PRINTER_HPP
