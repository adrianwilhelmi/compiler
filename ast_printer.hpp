#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

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

    void visit(ArrayAccessExpr& expr) override {
        printIndent();
        std::cout << "ArrayAccessExpr: " << expr.array_name << "[" << std::endl;
        indentLevel++;
        expr.index->accept(*this);
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
        std::cout << "AssignStmt: " << stmt.variable << " =" << std::endl;
        indentLevel++;
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
        for (auto* s : stmt.body) {
            s->accept(*this);
        }
        indentLevel--;
    }

    void visit(RepeatUntilStmt& stmt) override {
        printIndent();
        std::cout << "RepeatUntilStmt:" << std::endl;
        indentLevel++;
        for (auto* s : stmt.body) {
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
        for (auto* s : stmt.then_body) {
            s->accept(*this);
        }
        if (!stmt.else_body.empty()) {
            printIndent();
            std::cout << "Else:" << std::endl;
            for (auto* s : stmt.else_body) {
                s->accept(*this);
            }
        }
        indentLevel--;
    }

    void visit(ProcedureCallStmt& stmt) override {
        printIndent();
        std::cout << "ProcedureCallStmt: " << stmt.name << " with arguments:" << std::endl;
        indentLevel++;
        for (auto* arg : stmt.arguments) {
            arg->accept(*this);
        }
        indentLevel--;
    }

    void visit(ReadStmt& stmt) override {
        printIndent();
        std::cout << "ReadStmt: " << stmt.variable << std::endl;
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
        std::cout << "ProcedureDecl: " << proc.name << " with parameters:" << std::endl;
        indentLevel++;
        for (const auto& param : proc.parameters) {
            printIndent();
            std::cout << param << std::endl;
        }
        printIndent();
        std::cout << "Body:" << std::endl;
        for (auto* stmt : proc.body) {
            stmt->accept(*this);
        }
        indentLevel--;
    }

    void visit(Program& program) override {
        printIndent();
        std::cout << "Program:" << std::endl;
        indentLevel++;
        for (auto* proc : program.procedures) {
            proc->accept(*this);
        }
        printIndent();
        std::cout << "Main Body:" << std::endl;
        for (auto* stmt : program.main_body) {
            stmt->accept(*this);
        }
        indentLevel--;
    }
};


#endif //AST_PRINTER_HPP
