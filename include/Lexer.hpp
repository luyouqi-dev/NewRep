#ifndef LEXER
#define LEXER
#include <iostream>
#include <vector>
#include "OplType.hpp"
#include <algorithm>

class Lexer {
public:
	Lexer(std::string expr) { 
		this->expr = expr; 
		pos = -1; advance(); 
		ln = 1;
		col = 1;
		make_token(); 
	}
	std::vector<Token> tokens;
private: 
	std::string expr; 
	int pos; 
	char current;
	int ln, col;
	
	void advance() { 
		++pos; 
		col++;
		current = 0; 
		if (pos < (int)expr.size()) {
			current = expr[pos]; 
			if (current == '\n' || current == '\r') {
				tokens.push_back(Token {
					.type = TT_NEW_LINE,
					.data = "\n"
				});
				col = 1;
				ln++;
			}
		}
	}
	
	Token make_digit() {
		Token result; result.type = TT_INTEGER;
		char nxt = expr[pos + 1];
		int type = 0; // 0 -> DEC, 1 -> HEX, 2 -> BIN, 3 -> OCT
		if (current == '0' && nxt == 'x') {
			type = 1;
			advance();
			advance();
		} else if (current == '0' && nxt == 'b' || nxt == 'B') {
			type = 2;
		} else if (current == '0')
			type = 3;
		while (current != 0 && (std::isdigit(current) || current == '.' || current == 'x' || current == 'b' || current == 'B' || ('a' <= current && current <= 'z') || ('A' <= current && current <= 'Z'))) {
			if (current == '.')
				result.type = TT_DOUBLE;
			result.data += current;
			advance();
		}
		if (type == 1) result.data = hex_to_dec(result.data);
		else if (type == 2) result.data = bin_to_dec(result.data);
		else if (type == 3) result.data = oct_to_dec(result.data);
		return result;
	}

	Token make_ampersand() {
		Token result; 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char nxt = expr[new_pos];
			if (nxt == '&') {
				advance();
				advance();
				return Token{.type = TT_AND, .data = "&&"};
			}
		}
		advance();
		return Token{
			.type = TT_BIT_AND,
			.data = "&"
		};
	}

	Token make_pipe() {
		Token result; 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char nxt = expr[new_pos];
			if (nxt == '|') {
				advance();
				advance();
				return Token{.type = TT_OR, .data = "||"};
			}
		}
		advance();
		return Token{
			.type = TT_BIT_OR,
			.data = "|"
		};
	}
	
	Token make_string() {
		Token result;result.type = TT_STRING;
		char eof = current; advance();
		while (current != 0 && current != eof) {
			result.data += current;advance();}
		if (current != eof) throw std::runtime_error("error");
		advance();return result;
	}
	
	Token make_id() {
		Token result;result.type = TT_ID;
		while (current != 0 && IS_WORD(current) || isdigit(current)) {
			result.data += current;
			advance();
		}
		if (result.data == "true" || result.data == "false") {
			result.type = TT_INTEGER;
			result.data = result.data == "true"? "1" : "0";
			return result;
		}
		if (std::count(keys.begin(), keys.end(), result.data)) result.type = TT_KEY;
		return result;
	}
	
	Token make_plus() { 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char tmp = expr[new_pos];
			if (tmp == '=' || tmp == '+') {
				string a;
				a += current; advance();
				a += current; advance();
				return Token{.type = TT_SELFADD, .data = a};
			} else {
				advance(); 
				return Token{.type = TT_PLUS, .data = "+"};
			}
		} else {
			advance(); 
			return Token{.type = TT_PLUS, .data = "+"};
		}
	}

	Token make_sub() { 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char tmp = expr[new_pos];
			if (tmp == '=' || tmp == '-') {
				string a;
				a += current; advance();
				a += current; advance();
				return Token{.type = TT_SELFSUB, .data = a};
			} else {
				advance(); 
				return Token{.type = TT_SUB, .data = "-"};
			}
		} else {
			advance(); 
			return Token{.type = TT_SUB, .data = "-"};
		}
	}

	Token make_mul() { 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char tmp = expr[new_pos];
			if (tmp == '=') {
				string a;
				a += current; advance();
				a += current; advance();
				return Token{.type = TT_SELFMUL, .data = a};
			} else {
				advance(); 
				return Token{.type = TT_MUL, .data = "*"};
			}
		} else {
			advance(); 
			return Token{.type = TT_MUL, .data = "*"};
		}
	}

	Token make_percent() {
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char tmp = expr[new_pos];
			if (tmp == '=') {
				string a;
				a += current; advance();
				a += current; advance();
				return Token{.type = TT_SELF_MOD, .data = a};
			} else {
				advance(); 
				return Token{.type = TT_MOD, .data = "%"};
			}
		} else {
			advance(); 
			return Token{.type = TT_MOD, .data = "%"};
		}
	}

	Token make_div() { 
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char tmp = expr[new_pos];
			if (tmp == '=') {
				string a;
				a += current; advance();
				a += current; advance();
				return Token{.type = TT_SELFDIV, .data = a};
			} else {
				advance(); 
				return Token{.type = TT_DIV, .data = "/"};
			}
		} else {
			advance(); 
			return Token{.type = TT_DIV, .data = "/"};
		}
	}
	Token make_pow() { advance(); return Token{.type = TT_POW, .data = "^"}; }
	Token make_lbracket_s() { advance(); return Token{.type = TT_LBRACKET_S, .data = "("}; }
	Token make_rbracket_s() { advance(); return Token{.type = TT_RBRACKET_S, .data = ")"}; }
	Token make_lbracket_m() { advance(); return Token{.type = TT_LBRACKET_M, .data = "["}; }
	Token make_rbracket_m() { advance(); return Token{.type = TT_RBRACKET_M, .data = "]"}; }
	Token make_lbracket_b() { advance(); return Token{.type = TT_LBRACKET_B, .data = "{"}; }
	Token make_rbracket_b() { advance(); return Token{.type = TT_RBRACKET_B, .data = "}"}; }
	Token make_eq() { 
		int new_pos = pos + 1;
		if (new_pos >= expr.size()) {
			EQ: advance(); return Token{.type = TT_ASSIGN, .data = "="}; }
		char s = expr[new_pos];
		if (s != '=') goto EQ;
		advance(); advance();
		return Token{.type = TT_EQ, .data = "=="};
	}
	Token make_comma() { advance(); return Token{.type = TT_COMMA, .data = ","}; }
	Token make_period() { advance(); return Token{.type = TT_PERIOD, .data="."}; }
	
	Token make_biger() {
		if (pos < (int)expr.size() - 1) {
			char nxt = expr[pos+1];
			if (nxt == '=') {
				advance(); advance();
				return Token{.type = TT_BRE, .data = ">="};
			}
			if (nxt == '>') {
				advance(); advance();
				return Token {.type = TT_RIGHT_MOVE, .data = ">>"};
			}
		}
		advance(); return Token{.type = TT_BIGER, .data = ">"};
	}
	
	Token make_smaller() {
		if (pos < (int)expr.size() - 1) {
			char nxt = expr[pos+1];
			// <=
			if (nxt == '=') {
				advance(); advance();
				return Token{.type = TT_SRE, .data = "<="};
			}
			// <<
			if (nxt == '<') {
				advance(); advance();
				return Token {.type = TT_LEFT_MOVE, .data = "<<"};
			}
		}
		advance(); return Token{.type = TT_SMALLER, .data = "<"};
	}
	
	Token make_semicolon() { advance(); return Token{.type = TT_SEMICOLON, .data = ";"}; }

	Token make_not() {
		int new_pos = pos + 1;
		if (new_pos < expr.size()) {
			char nxt = expr[new_pos];
			if (nxt == '=') {
				advance();
				advance();
				return Token{
					.type = TT_NOT_EQ,
					.data = "!="
				};
			}
		}
		advance();
		return Token {
			.type = TT_NOT,
			.data = "!"
		};
	}
	
	Token make_colon() { advance(); return Token{.type = TT_COLON, .data = ":"}; }

	void skip() { while (current != 0 && current != '\n' && current != '\r') advance(); }
	
	void make_token() {
		while (current != 0) {
			if (IS_WORD(current)) tokens.push_back(make_id());
			else if (std::isdigit(current)) tokens.push_back(make_digit());
			else if (current == '"' || current == '\'') tokens.push_back(make_string());
			else if (current == '+') tokens.push_back(make_plus());
			else if (current == '.') tokens.push_back(make_period());
			else if (current == '!') tokens.push_back(make_not());
			else if (current == '-') tokens.push_back(make_sub());
			else if (current == ';') tokens.push_back(make_semicolon());
			else if (current == '|') tokens.push_back(make_pipe());
 			else if (current == '>') tokens.push_back(make_biger());
			else if (current == '<') tokens.push_back(make_smaller());
			else if (current == '*') tokens.push_back(make_mul());
			else if (current == '&') tokens.push_back(make_ampersand());
			else if (current == '#') skip();
			else if (current == ':') tokens.push_back(make_colon());
			else if (current == '/') tokens.push_back(make_div());
			else if (current == '^') tokens.push_back(make_pow());
			else if (current == '(') tokens.push_back(make_lbracket_s());
			else if (current == ')') tokens.push_back(make_rbracket_s());
			else if (current == '%') tokens.push_back(make_percent());
			else if (current == '=') tokens.push_back(make_eq());
			else if (current == '[') tokens.push_back(make_lbracket_m());
			else if (current == ']') tokens.push_back(make_rbracket_m());
			else if (current == ',') tokens.push_back(make_comma());
			else if (current == '{') tokens.push_back(make_lbracket_b());
			else if (current == '}') tokens.push_back(make_rbracket_b());
			else if (std::isspace(current)) advance();
			else {
				printf("Parse time error, at line %d, column %d, position %d: \n", ln, col, pos);
				printf("  Info: un supposed char '%c'\n", current);
				exit(-1);
			} 
		} 
	} 
};


#endif 