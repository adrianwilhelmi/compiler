
#include<memory>

#include"ast_printer.hpp"
#include"ast.hpp"

/*
int main() {
    auto expr1 = std::make_unique<NumberExpr>(42);
    auto expr2 = std::make_unique<VariableExpr>("x");
    auto binaryExpr = std::make_unique<BinaryOpExpr>("+", std::move(expr1), std::move(expr2));

    auto stmt = std::make_unique<AssignStmt>("y", std::move(binaryExpr));
    std::vector<std::unique_ptr<Statement>> mainBody;
    mainBody.push_back(std::move(stmt));

    Program program({}, std::move(mainBody));

    ASTPrinter printer;
    program.accept(printer);

    return 0;
}
*/


//author: ChatGPT (prompt engineer: Adrian Wilhelmi)

// Przykład użycia ASTPrinter
int main() {
    // 1. Prosty test z przypisaniem
    auto expr1 = std::make_unique<NumberExpr>(42);
    auto expr2 = std::make_unique<VariableExpr>("x");
    auto binaryExpr = std::make_unique<BinaryOpExpr>("+", std::move(expr1), std::move(expr2));
    auto stmt1 = std::make_unique<AssignStmt>("y", std::move(binaryExpr));
    std::vector<std::unique_ptr<Statement>> mainBody;
    mainBody.push_back(std::move(stmt1));

    // 2. Pętla for z "downto"
    auto fromExpr = std::make_unique<NumberExpr>(10);
    auto toExpr = std::make_unique<NumberExpr>(1);
    std::vector<std::unique_ptr<Statement>> loopBody;
    loopBody.push_back(std::make_unique<WriteStmt>(std::make_unique<VariableExpr>("i")));
    auto forStmt = std::make_unique<ForStmt>("i", std::move(fromExpr), std::move(toExpr), true);
    forStmt->body = std::move(loopBody);
    mainBody.push_back(std::move(forStmt));

    // 3. Instrukcja warunkowa if-else
    auto condExpr = std::make_unique<BinaryOpExpr>(">", std::make_unique<VariableExpr>("y"), std::make_unique<NumberExpr>(0));
    std::vector<std::unique_ptr<Statement>> thenBody;
    thenBody.push_back(std::make_unique<WriteStmt>(std::make_unique<VariableExpr>("y")));
    std::vector<std::unique_ptr<Statement>> elseBody;
    elseBody.push_back(std::make_unique<WriteStmt>(std::make_unique<NumberExpr>(0)));
    auto ifStmt = std::make_unique<IfStmt>(std::move(condExpr), std::move(thenBody), std::move(elseBody));
    mainBody.push_back(std::move(ifStmt));

    // 4. Repeat-Until
    auto repeatCond = std::make_unique<BinaryOpExpr>("==", std::make_unique<VariableExpr>("x"), std::make_unique<NumberExpr>(10));
    std::vector<std::unique_ptr<Statement>> repeatBody;
    repeatBody.push_back(std::make_unique<WriteStmt>(std::make_unique<VariableExpr>("x")));
    repeatBody.push_back(std::make_unique<AssignStmt>("x", std::make_unique<BinaryOpExpr>("+", std::make_unique<VariableExpr>("x"), std::make_unique<NumberExpr>(1))));
    auto repeatStmt = std::make_unique<RepeatUntilStmt>(std::move(repeatBody), std::move(repeatCond));
    mainBody.push_back(std::move(repeatStmt));

    // 5. Wywołanie procedury z argumentami
    std::vector<std::unique_ptr<Expression>> procArgs;
    procArgs.push_back(std::make_unique<NumberExpr>(5));
    procArgs.push_back(std::make_unique<VariableExpr>("y"));
    auto procCallStmt = std::make_unique<ProcedureCallStmt>("doSomething", std::move(procArgs));
    mainBody.push_back(std::move(procCallStmt));

    // 6. Procedura
    std::vector<std::unique_ptr<Statement>> procBody;
    procBody.push_back(std::make_unique<AssignStmt>("result", std::make_unique<BinaryOpExpr>("*", std::make_unique<VariableExpr>("a"), std::make_unique<VariableExpr>("b"))));
    procBody.push_back(std::make_unique<WriteStmt>(std::make_unique<VariableExpr>("result")));
    auto procedure = std::make_unique<ProcedureDecl>("multiply", std::vector<std::string>{"a", "b"}, std::move(procBody));

    std::vector<std::unique_ptr<ProcedureDecl>> procedures;
    procedures.push_back(std::move(procedure));

    // Tworzenie programu
    Program program(std::move(procedures), std::move(mainBody));

    // Drukowanie AST
    ASTPrinter printer;
    program.accept(printer);

    return 0;
}
