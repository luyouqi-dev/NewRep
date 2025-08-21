//
// Created by User on 2025/7/21.
//

#ifndef CPROGRAMLANGUAGE_TOOLS_HPP
#define CPROGRAMLANGUAGE_TOOLS_HPP
#include <fstream>
#include "Lexer.hpp"
#include "color256.hpp"
#include "VBox.hpp"
#include <iostream>
#define ATL(x) ((long long)(&(x)))
#define PTL(x) ((long long)(x))

bool isdigit(string a) {
	for (auto i : a)
		if (!isdigit(i))
			return false;
	return true;
}

string read_file(string path) {
	string res, buffer;
	ifstream ifs(path);
	while (getline(ifs, buffer))
		res += buffer + '\n';
	return res;
}

#endif //CPROGRAMLANGUAGE_TOOLS_HPP

