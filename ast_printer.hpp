#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include<iostream>

#include"ast.hpp"
#include"ast_visitor.hpp"

// author: ChatGPT (prompt engineer: Adrian Wilhelmi)

// Klasa wizytora do debugowania i drukowania AST
struct ASTPrinter : ASTVisitor {
    int indentLevel = 0;

    // Funkcja pomocnicza do drukowania wcięć
    void printIndent() {
        for (int i = 0; i < indentLevel; i++) std::cout << "  ";
    }

    // Funkcja pomocnicza do drukowania wcięcia przed każdą wizytą
    void increaseIndent() {
        indentLevel++;
    }

    // Funkcja pomocnicza do obniżania wcięcia
    void decreaseIndent() {
        indentLevel--;
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
        increaseIndent();
        printIndent();
        std::cout << expr.index << std::endl;
        decreaseIndent();
        printIndent();
        std::cout << "]" << std::endl;
    }

    void visit(ArrayAccessWithNumExpr& expr) override {
        printIndent();
        std::cout << "ArrayAccessExpr: " << expr.array_name << "[" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << expr.index << std::endl;
        decreaseIndent();
        printIndent();
        std::cout << "]" << std::endl;
    }

    void visit(BinaryOpExpr& expr) override {
        printIndent();
        std::cout << "BinaryOpExpr: " << expr.op << std::endl;
        increaseIndent();
        expr.left->accept(*this);
        expr.right->accept(*this);
        decreaseIndent();
    }

    void visit(AssignStmt& stmt) override {
        printIndent();
        std::cout << "AssignStmt: " << std::endl;
        increaseIndent();
        stmt.variable->accept(*this);
        stmt.value->accept(*this);
        decreaseIndent();
    }

    void visit(ForStmt& stmt) override {
        printIndent();
        std::cout << "ForStmt: " << stmt.iterator << " from" << std::endl;
        increaseIndent();
        stmt.from->accept(*this);
        printIndent();
        std::cout << (stmt.downto ? "downto" : "to") << std::endl;
        stmt.to->accept(*this);
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
        decreaseIndent();
    }

    void visit(RepeatUntilStmt& stmt) override {
        printIndent();
        std::cout << "RepeatUntilStmt:" << std::endl;
        increaseIndent();
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
        printIndent();
        std::cout << "Until:" << std::endl;
        stmt.condition->accept(*this);
        decreaseIndent();
    }

    void visit(IfStmt& stmt) override {
        printIndent();
        std::cout << "IfStmt:" << std::endl;
        increaseIndent();
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
        decreaseIndent();
    }

    void visit(ProcedureCallStmt& stmt) override {
        printIndent();
        std::cout << "ProcedureCallStmt: " << stmt.name << " with arguments:" << std::endl;
        increaseIndent();
        for (auto& arg : stmt.arguments) {    
            printIndent();
            std::cout << arg << ", ";
        }
        std::cout << std::endl;
        decreaseIndent();
    }

    void visit(ReadStmt& stmt) override {
        printIndent();
        std::cout << "ReadStmt:" << std::endl;
        increaseIndent();
        stmt.variable->accept(*this);
        decreaseIndent();
    }

    void visit(WriteStmt& stmt) override {
        printIndent();
        std::cout << "WriteStmt:" << std::endl;
        increaseIndent();
        stmt.value->accept(*this);
        decreaseIndent();
    }

    void visit(ProcedureDecl& proc) override {
        printIndent();
        std::cout << "ProcedureDecl: " << std::endl;
        increaseIndent();
        proc.head->accept(*this);

        for (const auto& param : proc.parameters) {
            param->accept(*this);    
        }
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto& stmt : proc.body) {
            stmt->accept(*this);
        }
        decreaseIndent();
    }

    void visit(Program& program) override {
        printIndent();
        std::cout << "Program:" << std::endl;
        increaseIndent();
        for (auto& proc : program.procedures) {
            proc->accept(*this);
        }
        printIndent();
        program.main->accept(*this);
        decreaseIndent();
    }

    void visit(ArrayDeclarationExpr& expr) override {
        printIndent();
        std::cout << "ArrayDeclarationExpr:" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << "Variable: " << expr.var << std::endl;
        printIndent();
        std::cout << "From:" << expr.from << std::endl;
        printIndent();
        std::cout << "To:" << expr.to << std::endl;
        decreaseIndent();
    }

    void visit(VariableDeclarationExpr& expr) override {
        printIndent();
        std::cout << "VariableDeclarationExpr:" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << "Variable: " << expr.var << std::endl;
        decreaseIndent();
    }

    void visit(VariableArgDeclExpr& expr) override {
        printIndent();
        std::cout << "VariableArgDeclExpr:" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << "Variable: " << expr.var << std::endl;
        decreaseIndent();
    }

    void visit(ArrayArgDeclExpr& expr) override {
        printIndent();
        std::cout << "ArrayArgDeclExpr:" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << "Variable: " << expr.var << std::endl;
        decreaseIndent();
    }


    void visit(ProcedureHeadExpr& expr) override {
        printIndent();
        std::cout << "ProcedureHeadExpr: " << std::endl;
        increaseIndent();
        printIndent();
        std::cout << expr.proc_name << std::endl;
        printIndent();
        std::cout << "Parameters:" << std::endl;
        increaseIndent();
        for (auto& var : expr.vars) {
		var->accept(*this);
        }
        std::cout << std::endl;
        decreaseIndent();
        decreaseIndent();
    }

    void visit(ConditionExpr& expr) override {
        printIndent();
        std::cout << "ConditionExpr: " << expr.op << std::endl;
        increaseIndent();
        expr.left->accept(*this);
        expr.right->accept(*this);
        decreaseIndent();
    }

    void visit(WhileStmt& stmt) override {
        printIndent();
        std::cout << "WhileStmt:" << std::endl;
        increaseIndent();
        printIndent();
        std::cout << "Condition:" << std::endl;
        stmt.condition->accept(*this);
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto& s : stmt.body) {
            s->accept(*this);
        }
        decreaseIndent();
    }

    void visit(MainProcedure& mainProcedure) {
        printIndent();
        std::cout << "MainProcedure {" << std::endl;

        // Drukuj deklaracje
        printIndent();
        std::cout << "  Declarations:" << std::endl;
        for (const auto& decl : mainProcedure.declarations) {
            decl->accept(*this); // Odwiedź każdą deklarację
        }

        // Drukuj ciało
        printIndent();
        std::cout << "  Body:" << std::endl;
        for (const auto& stmt : mainProcedure.body) {
            stmt->accept(*this); // Odwiedź każdą instrukcję
        }

        printIndent();
        std::cout << "}" << std::endl;
    }

    void visit(MainProcedureNoDecl& mainProcedureNoDecl) {
        printIndent();
        std::cout << "MainProcedureNoDecl" << std::endl;

        // Drukuj ciało
        printIndent();
        std::cout << "  Body:" << std::endl;
        for (const auto& stmt : mainProcedureNoDecl.body) {
            stmt->accept(*this); // Odwiedź każdą instrukcję
        }
    }
};

#endif //AST_PRINTER_HPP
