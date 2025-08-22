#include <iostream>
#include <ctime>
#include <cstdio>
#include "include/VBox.hpp"
#include "include/Compiler.hpp"
#include <vector>
#include "include/Tools.hpp"
#include "include/color256.hpp"
#include <fstream>
#include <filesystem>
#include "include/Lexer.hpp"
#include "include/Tools.hpp"
#include "include/Parser.hpp"
using namespace std;

void shell() {
	while (true) {
		string expr;
		printf(">>> ");
		getline(cin, expr);
		Lexer lexer(expr);
		Parser parser(lexer.tokens);
		auto res = parser.result.ast;
		for (auto i : res)
			print_tree(i);
	}
}

void file() {
#ifdef _WIN32
	string default_file = R"(D:\CLionProjects\ProgramLanguageInCpp\script\exam.opl)";
	string file1 = R"(D:\CLionProjects\ProgramLanguageInCpp\script\test1.opl)";
	string file2 = R"(D:\CLionProjects\ProgramLanguageInCpp\script\test2.opl)";
	string data = read_file(file1);
	Lexer lexer(data);
	Parser parser(lexer.tokens);
	auto res = parser.result.ast;
	auto ptcr = parser.ptcr;
	for (auto i = ptcr.begin(); i != ptcr.end(); ++i) {
		cout << i->first << ": \n";
		auto j = i->second.var_type_record;
		for (auto k = j.begin(); k != j.end(); ++k) {
			cout << "\t" << k->first << ": " << k->second.rootType << endl;
		}
	}
	for (auto i : res) {
		print_tree(i);
	}
	auto cmp = [&](){
		Compiler tac(res);
		auto cmds = tac.get_compile_result().opcs;
		printf("\nThreeCodes=======================================\n");
		for (auto i: cmds) {
			Color256output(i.label_name, 3, 12, 0);
			printf(": ");
			for (auto j: i.codes)
				cout << j << " ";
			cout << endl;
		}
	};
	cmp();
#endif
#ifdef __linux__
	cout << "LINUX" << endl;
	string default_file = R"(./script/exam.opl)";
	string file1   = R"(./script/test1.opl)";
	string file2 = R"(./script/test2.opl)";
	string data = read_file(file1);
	Lexer lexer(data);
	Parser parser(lexer.tokens);
	Compiler comp(parser.result.ast);
	auto cmds = comp.get_compile_result().opcs;
	for (auto i : cmds) {
		Color256output(i.label_name, 3, 12, 0);
		printf(": ");
	 	for (auto j : i.codes)
	 		cout << j <<  " ";
	 	cout << endl;
	}
#endif
}

int proc_args(int argc, char** argv) {
#ifdef _WIN32
	if (argc <= 1) {
		cerr << "Usage" << argv[0] << " <fileName>" << endl;
		return 1;
	}
	
	try {
		string inputFile = argv[1];
		string data = read_file(inputFile);
		if (data.empty()) {
			throw runtime_error("can not read");
		}
		
		Lexer lexer(data);
		Parser parser(lexer.tokens);
		Compiler tac(parser.result.ast);
		auto cmds = tac.get_compile_result().opcs;
		namespace fs = std::filesystem;
		fs::path outputPath = fs::path(inputFile).stem();
		outputPath += "_three_code";
		ofstream ofs(outputPath);
		if (!ofs) {
			throw runtime_error("can not output" + outputPath.string());
		}
		for (auto cmd : cmds) {
			ofs << cmd.label_name << ": ";
			for (auto code : cmd.codes) {
				ofs << code << " ";
			}
			ofs << "\n";
		}
		cout << "file: " << outputPath << endl;
	} catch (exception e) {
		cerr << "error: " << e.what() << endl;
		return 1;
	}
#endif
#ifdef __linux__
	cout << "compiler in linux" << endl;
	string input = argv[1];
	string data = read_file(input);
	Lexer lex(data);
	Parser par(lex.tokens);
	Compiler com(par.result.ast);
	auto cmd = com.get_compile_result().opcs;
	for (auto i : cmd) {
		Color256output(i.label_name, 3, 12, 0);
		printf(": ");
	 	for (auto j : i.codes)
	 		cout << j <<  " ";
	 	cout << endl;
	}
#endif
	return 0;
}

void vm_test() {
	vector<int> assembly = {};
	VirtualMachine vm(assembly);
}

int main(int argc, char **argv) {
	map<string, int> a;
	a["hello"] = 0;
	int& b = a["hello"];
	a["hello"] = 21;
	cout << b << endl;
	return 0;
}

