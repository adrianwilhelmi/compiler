//author: Adrian Wilhelmi

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

	FILE* input_file = fopen(argv[1], "r");
	if(!input_file){
		std::cerr << "error: cannot open file " << argv[1] << std::endl;
		return 1;
	}

	yyin = input_file;
	calc::Parser parser;

	if(parser.parse() == 0){
		if(root){
			std::cout << "ast succesfully built:\n";
			ASTPrinter printer;
			root->accept(printer);
		}
		else{
			std::cerr << "error: ast not created" << std::endl;
		}
	}
	else{
		std::cerr << "error: parsing failed" << std::endl;
	}

	fclose(input_file);

	return 0;
}
