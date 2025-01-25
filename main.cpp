#include"ast.hpp"
#include"ast_printer.hpp"
#include"parser.hpp"
#include"lexer.hpp"

#include<fstream>
#include<iostream>
#include<memory>
#include<vector>
#include<string>

std::unique_ptr<Program> root;

int main(int argc, char*argv[]){
	if(argc < 2){
		std::cerr << "usage: " << argv[0] << " <input_file>" << std::endl;
		return 1;
	}

	std::ifstream input_file(argv[1]);
	if(!input_file.is_open()){
		std::cerr << "error: cannot open file " << argv[1] << std::endl;
		return 1;
	}

	yyscan_t scanner;	
	yylex_init(&scanner);
	yyset_in(input_file, scanner);

	calc::Parser parser{ scanner };
	parser.parse();

	if(root){
		std::cout << "ast succesfully built:\n";
		ASTPrinter printer;
		root->accept(printer);
	}
	else{
		std::cerr << "error: ast not created" << std::endl;
	}

	yylex_destroy(scanner);

	return 0;
}
