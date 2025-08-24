#ifndef CPROGRAMLANGUAGE_PARSER_HPP
#define CPROGRAMLANGUAGE_PARSER_HPP
#include <iostream>
#include "BuildIn.hpp"
#include <vector>
#include "Lexer.hpp"
#include <unordered_map>
#include "Tools.hpp"
#include <stack>
#include <algorithm>
#include "OplType.hpp"
using std::stack;

class Parser;

typedef AST* (Parser::*func)();

struct VarList {
	vector<string> v;
	map<string, SymbolKind> symbol_kinds;
	map<string, IdTypeNode> var_type_record;
	vector<string> current_class;
	bool is_type(string);
	void add(string, SymbolKind);
	void add_var(string, IdTypeNode);
	bool is_have(string);
	bool is_haveA(string, SymbolKind);
};

void VarList::add_var(string name, IdTypeNode type) {
	var_type_record[name] = type;
}

bool VarList::is_type(std::string name) {
	return is_haveA(name, SK_CLASS);
}

bool VarList::is_haveA(std::string name, SymbolKind kind) {
	if (is_have(name) && symbol_kinds[name] == kind)
		return true;
	return false;
}

void VarList::add(std::string s, SymbolKind kd) {
	if (std::find(v.begin(), v.end(), s) != v.end()) {
		err_out(PARSE_TIME_ERROR, "var '%s' double define", s.c_str());
	}
	symbol_kinds[s] = kd;
	v.push_back(s);
}

bool VarList::is_have(std::string s) {
	return std::find(v.begin(), v.end(), s) != v.end();
}

template<class T>
bool is_exist(vector<T> v, T vl) {
	for (auto i : v)
		if (i == vl)
			return true;
	return false;
}

class Parser {
public:
	Parser(std::vector<Token> tokens, int file = 1) {
		_tokens = tokens;
		offset = 0;
		export_map = &result.export_map;
		lin = col = 1;
		pos = -1;
		advance();
		for (const auto& i : build_in_class) class_map[i] = make_class_id();
		for (const auto& i : build_in_class) class_size_record[i] = 1;
		scope.emplace_back();
		for (const auto& i : build_in_class) add_to_global_scope(i, SK_CLASS);
		main_parse();
	}
	int offset;
	int lin, col;
	AST* asr;
	Program result;
	void main_parse();
	map<string, int> class_size_record;
	unordered_map<string, ParseTimeClassRecord> ptcr;
	unordered_map<string, int> class_map;
	unordered_map<string, ExportId>* export_map;
	unordered_map<string, SymbolId> id_map;
	VarList global;
	int class_cnt, func_cnt, var_cnt;
	int make_class_id();
	int pos;
	map<string, int> var_map;
	map<string, int> func_map;
	// Var ->  1
	// Func -> 2
	// Class ->3
	vector<VarList> scope;
private:
	void leave();
	void crate();
	bool is_define(string);
	void check_type(AST*);
	bool is_defineA(string, SymbolKind);
	void add(string, SymbolKind);
	void add_var(string ,IdTypeNode);
	IdTypeNode get_type(string);
	void consume(std::string, std::string);
	void consume_type(std::string, std::string);
	Token* current;
	AST* make_string_node();
	std::vector<Token> _tokens;
	void advance();
	AST* make_if();
	AST* make_while();
	AST* make_a_assign();
	AST* make_for();
	AST* make_case_node();
	AST* make_default_node();
	AST* make_switch_node();
	vector<AST*> make_templates();
	AST* make_load_library();
	AST* make_else();
	AST* make_return_node();
	AST* make_var_define();
	AST* abs_call();
	AST* make_assign(AST*);
	AST* make_mem_malloc();
	AST* make_block();
	AST* atom_make(bool&);
	AST* make_member_node();
	AST* make_class();
	AST* make_inc_node(AST*);
	AST* make_dec_node(AST*);
	AST* make_id_atom();
	AST* make_list();
	int get_class_size(AST*);
	AST* make_digit();
	bool match_data(std::string);
	AST* make_not_node();
	AST* make_bit_node();
	AST* make_lambda();
	AST* make_type_node();
	AST* make_term();
	Token want_get(string);
	AST* make_load_module_node();
	AST* make_templates(Token);
	AST* make_expr_term();
	AST* make_constructor_call(AST*);
	AST* make_constructor_call_();
	AST* make_expr();
	AST* make_get_value_node(AST*);
	AST* make_and_node();
	AST* make_comp_node();
	AST* make_self_add_node(AST*);
	AST* make_self_sub_node(AST*);
	AST* make_break();
	AST* make_continue();
	AST* make_self_div_node(AST*);
	AST* make_import_node();
	AST* make_self_mul_node(AST*);
	AST* make_function_define();
	AST* make_cast_node(AST*);
	AST* make_or_node();
	AST* atom();
	AST* make_func_call(AST*);
	void make_lib_class(Token);
	bool match_type(std::string);
	void add_to_global_scope(string, SymbolKind);
	AST* make_bin_op_node(func, std::vector<string>, func);
	bool is_class_member(Token);
	string get_mem_type(string, string);
	bool is_type(AST*);
};

void Parser::make_lib_class(Token t) {
	string path = t.data;
	string data = read_file(path);
}

string Parser::get_mem_type(string class_name, string mem_name) {
	return ptcr[class_name].get_root_type(mem_name);
}

IdTypeNode Parser::get_type(string s) {
	for (auto i : scope)
		if (i.var_type_record.find(s) != i.var_type_record.end())
			return i.var_type_record[s];
	return scope[scope.size() - 1].var_type_record[s];
}

void Parser::add_var(string vn, IdTypeNode vt) {
	scope[scope.size() - 1].add_var(vn, vt);
}

AST *Parser::make_bin_op_node(func left, std::vector<string> ops, func right = nullptr) {
	if (!right) right = left;
	AST* _left = (this->*left)();
	while (current && std::count(ops.begin(), ops.end(), current->data) > 0) {
		Token op = *current;
		advance();
		AST* _right = (this->*right)();
		_left = new BinOpNode(op, _left, _right);
	}
	return _left;
}

AST* Parser::make_bit_node() { return make_bin_op_node(&Parser::make_expr_term, {"<<", ">>", "|", "&"}); }

bool Parser::is_defineA(std::string name, SymbolKind kind) {
	for (auto i : scope) if (i.is_haveA(name, kind)) return true;
	return false;
}

void Parser::add_to_global_scope(string s, SymbolKind kd) { scope[0].add(s, kd); }

AST* Parser::make_templates(Token id) {
	vector<AST*> template_list;
	if (match_data("<")) {
		advance();
		while (current && !match_data(">")) {
			current->debug();
			auto tmp = make_type_node();
			add(tmp->data.data, SK_CLASS);
			template_list.push_back(tmp);
			if (match_data(">")) break;
			consume(",", __func__ );
		}
		consume(">", __func__ );
	}
	return new TypeNode(id, template_list);
}

AST* Parser::make_type_node() {
	auto root_type = *current;
	bool is_block = false;
	AST* block_size = nullptr;
	advance();
	vector<AST*> template_list;
	if (match_data("<")) {
		advance();
		while (current && !match_data(">")) {
			template_list.push_back(make_type_node());
			if (match_data(">")) break;
			consume(",", __func__ );
		}
		consume(">", __func__ );
	}
	if (match_data("[")) {
		is_block = true;
		advance();
		block_size = make_expr();
		consume("]", __func__ );
	}
	auto n = new TypeNode(root_type, template_list, is_block, block_size);
	check_type(n);
	return n;
}

bool Parser::is_class_member(Token a) {
	return class_map.find(a.data) != class_map.end() || is_defineA(a.data, SK_CLASS);
}

AST* Parser::make_cast_node(AST* target_type) {
	auto expr_ = make_expr();
	return new CastNode(target_type, expr_);
}

void Parser::advance() {
	++pos;
	++col;
	current = nullptr;
	if (pos < _tokens.size()) {
		current = &_tokens[pos];
		if (current->type == TT_NEW_LINE) {
			col = 1;
			lin++;
			advance();
		}
	}
}

string get_string(AST* a) {
	if (a->type == AST_ID)
		return a->data.data;
	if (a->type == AST_GET_VALUE)
		return get_string(a->children[0]);
	if (a->type == AST_MEMBER) {
		auto base = a->children[0];
		auto member = a->children[1];
		return get_string(base);
	}
}

AST* Parser::make_member_node() {
	AST* base = make_id_atom();
	string type;
	if (base->type == AST_TYPE)
		return base;
	if (base->children.size() > 0 && base->type == AST_GET_VALUE) {
		if (base->type != AST_ID) type = get_type(base->children[0]->data.data).templateType[0].rootType;
		else type = get_type(base->data.data).templateType[0].rootType;
	}
	else type = get_type(base->data.data).rootType;
	while (current && match_data(".")) {
		consume(".", __func__ );
		AST* member = make_id_atom();
		if (member->type == AST_TYPE)
			return member;
		base = new MemberNode(base, member);
		if (member->children.size() > 0 && member->type == AST_GET_VALUE) {
			string data_t;
			if (base->type != AST_MEMBER) {
				data_t = base->children[0]->data.data;
				base->parent_name = get_type(data_t).templateType[0].rootType;
			}
			else {
				base->parent_name = type;
			}
		} else {
			base->parent_name = type;
		}
		type = ptcr[type].get_root_type(member->data.data);
	}
	if (match_data("[")) base = make_get_value_node(base);
	return base;
}


void Parser::consume(std::string s, string call_id) {
	if (!current)
		err_out(PARSE_TIME_ERROR, "Unexpected EOF in lin %d, col %d, pos %d", lin, col, pos);
	if (current->data == s) { advance(); return; }
	err_out(PARSE_TIME_ERROR, "want '%s', meet '%s', at lin %d, col %d, pos %d", s.c_str(), current->data.c_str(), lin, col, pos);
}

vector<AST*> Parser::make_templates() {
	vector<AST*> temps;
	consume("<", __func__);
	while (current && !match_data(">")) {
		if (current->type != TT_ID) {
			err_out(PARSE_TIME_ERROR, "Expected template parameter name at lin %d, col %d, pos %d", lin, col, pos);
		}
		Token param = *current;
		advance();
		add(param.data, SK_CLASS);
		temps.push_back(new IdNode(param));
		if (match_data(">")) break;
		consume(",", __func__);
	}
	consume(">", __func__);
	return temps;
}

AST *Parser::make_id_atom() {
	if (!current) return nullptr;
	AST* idp = new IdNode(*current);
	advance();
	auto nxt = _tokens[pos + 1];
	bool is_t = class_map.find(idp->data.data) != class_map.end();
	if (is_t && match_data("<") && class_map[nxt.data]) {
		auto temp = make_templates();
		idp->children = temp;
	}
	if (!is_t) {
		while (match_data("["))
			idp = make_get_value_node(idp);
	} else {
		if (match_data("[")) {
			auto si = make_expr();
			consume("]", __func__ );
			return new TypeNode(idp->data, true, si);
		}
		return new TypeNode(idp->data);
	}
	return idp;
}

bool Parser::match_data(std::string d) {
	if (!current) return false;
	return current->data == d;
}

bool Parser::match_type(std::string t) {
	if (!current) return false;
	return current->type == t;
}

AST* Parser::make_term() {
	return make_bin_op_node(&Parser::atom, {"+", "-"});
}

AST *Parser::atom() {
	if (match_data("lambda")) {
		return make_lambda();
	} if (match_data("-")) {
		advance();
		auto num = make_digit();
		num->data.data = "-" + num->data.data;
		return num;
	} if (match_data("load_library")) {
		return make_load_library();
	} if (match_data("["))
		return make_list();
	if (match_data("!")) {
		return make_not_node();
	} if (match_data("++")) {
		advance();
		auto id = make_member_node();
		return make_inc_node(id);
	} if (match_data("--")) {
		advance();
		auto id = make_member_node();
		return make_dec_node( id);
	} if (match_type(TT_KEY)) {
		if (match_data("new")) {
			return make_mem_malloc();
		}
		else {
			err_out(PARSE_TIME_ERROR, "Key '%s' not supposed, at lin %s, col %d, pos %d", current->data.c_str(), lin, col, pos);
		}
	}
	if (match_type(TT_DOUBLE) || match_type(TT_INTEGER)) return make_digit();
	if (match_type(TT_ID)) {
		auto tmp = make_member_node();
		string data = tmp->data.data;
		if (!is_define(data) && match_type(TT_ID)) {
			printf("Error: symbol '%s' is not define in this scope\n", data.c_str());
			printf("At lin %d, col %d, pos %d\n", lin, col, pos);
			printf("Current data: ");
			current->debug();
			exit(-1);
		}
		if (match_data("=")) {
			return make_assign(tmp);
		} if (match_data("("))
			return make_func_call(tmp);
		if (match_data("++")) {
			advance();
			return make_inc_node(tmp);
		} if (match_data("--")) {
			advance();
			return make_dec_node(tmp);
		}
		return tmp;
	}
	if (match_type(TT_STRING)) return make_string_node();
	consume("(", __func__ );
	auto tok = *current;
	auto tmp = make_expr();
	if (is_class_member(tok)) {
		auto tp = make_templates(tok);
		consume(")", __func__ );
		return make_cast_node(tp);
	}
	consume(")", __func__ );
	return tmp;
}

AST *Parser::make_digit() {
	auto ni = new DigitNode(*current);
	advance();
	return ni;
}

AST *Parser::make_expr_term() {
	return make_bin_op_node(&Parser::make_term, {"/", "*", "%"});
}

AST *Parser::make_string_node() {
	auto tmp = new StringNode(*current);
	advance();
	return tmp;
}

AST *Parser::make_var_define() {
	// printf("=========================================================\n");
	++var_cnt;
	Token var_name;
	AST* type = nullptr;
	AST* data = nullptr;
	if (match_data("let"))
		advance();
	if (!match_type(TT_ID)) {
		err_out(PARSE_TIME_ERROR, "want TT_ID, meet %s, lin %d, col %d, pos %d", current->type.c_str(), lin, col, pos);
	}
	var_name = *current;
	var_map[var_name.data] = var_cnt;
	add(var_name.data, SK_VAR);
	advance();
	if (match_data(":")) {
		advance();
		type = make_type_node();
		check_type(type);
	} if (match_data("=")) {
		advance();
		data = make_expr();
	}
	if (type) {
		auto type_ = make_id_type_node(type);
		scope[scope.size() - 1].add_var(var_name.data, type_);
	}
	return new VarDefineNode(var_name, type, data);
}

AST *Parser::make_and_node() {
	return make_bin_op_node(&Parser::make_comp_node, {"and", "&&"});
}

AST* Parser::make_or_node() {
	return make_bin_op_node(&Parser::make_and_node, {"or", "||"});
}

AST *Parser::make_comp_node() {
	return make_bin_op_node(&Parser::make_bit_node, {"<", ">", "<=", ">=", "==", "!="});
}

AST *Parser::make_expr() {
	return make_or_node();
}

AST *Parser::make_func_call(AST* name) {
	vector<AST*> value;
	consume("(", __func__ );
	while (current && !match_data(")")) {
		value.push_back(make_expr());
		if (match_data(")")) break;
		consume(",", __func__ );
	}
	consume(")", __func__ );
	return new FunctionCallNode(name, value);
}

void Parser::consume_type(std::string type, std::string func) {
	if (!current || current->type != type) {
		err_out(PARSE_TIME_ERROR, "want %s, meet %s, at lin %d, col %d, pos %d", type.c_str(), current->data.c_str(), lin, col, pos);
	}
}

AST *Parser::abs_call() {
	auto mn = make_id_atom();
	auto res = make_func_call(mn);
	return res;
}

AST *Parser::atom_make(bool& should_split) {
	if (match_data("++")) {
		should_split = true;
		advance();
		auto id = make_member_node();
		return make_inc_node(id);
	} if (match_data("--")) {
		should_split = true;
		advance();
		auto id = make_member_node();
		return make_dec_node(id);
	} if (match_type(TT_KEY)) {
		if (match_data("let")) {
			auto tmp = make_var_define();
			should_split = true;
			return tmp;
		} else if (match_data("export")) {
			advance();
			if (match_data("function")) {
				should_split = false;
				auto node = make_function_define();
				(*export_map)[node->data.data] = FUNCTION_EXPORT;
				return node;
			} else if (match_data("let")) {
				should_split = true;
				auto node = make_var_define();
				string name = node->data.data;
				(*export_map)[node->data.data] = VAR_EXPORT;
				return node;
			} else if (match_data("class")) {
				auto node = make_class();
				(*export_map)[node->data.data] = CLASS_EXPORT;
				should_split = false;
				return node;
			} else {
				printf("Parse time error, at lin %d, col %d, pos %d, when export\n", lin, col, pos);
				exit(-1);
			}
		} else if (match_data("module")) {
			should_split = true;
			return make_load_module_node();
		} else if (match_data("using")) {
			should_split = true;
			return make_import_node();
		} else if (match_data("continue")) {
			should_split = true;
			return make_continue();
		} else if (match_data("switch")) {
			should_split = false;
			return make_switch_node();
		} else if (match_data("break")) {
			should_split = true;
			return make_break();
		} else if (match_data("return")) {
			should_split = true;
			auto node = make_return_node();
			return node;
		} else if (match_data("class")) {
			should_split = false;
			return make_class();
		} else if (match_data("for")) {
			should_split = false;
			return make_for();
		} else if (match_data("while")) {
			return make_while();
		} else if (match_data("if")) {
			should_split = false;
			return make_if();
		} else if (match_data("function")) {
			should_split = false;
			return make_function_define();
		} else {
			err_out(PARSE_TIME_ERROR, "Key '%s' not supposed at lin %s, col %d, pos %d", current->data.c_str(), lin, col, pos);
		}
	} else if (match_type(TT_ID)) {
		should_split = true;
		auto id = make_member_node();
		if (match_data("++")) { advance(); return make_inc_node(id); }
		if (match_data("--")) { advance(); return make_dec_node(id); }
		if (match_data("+=")) return make_self_add_node(id);
		if (match_data("-=")) return make_self_sub_node(id);
		if (match_data("/=")) return make_self_div_node(id);
		if (match_data("*=")) return make_self_mul_node(id);
		if (match_data("=")) return make_assign(id);
		if (match_data("(")) return make_func_call(id);
		return id;
	}
	return nullptr;
}

AST *Parser::make_block() {
	vector<AST*> tmp;
	if (match_data("{")) {
		consume("{", __func__);
		while (current && !match_data("}")) {
			if (match_data("}")) break;
			bool isr = false;
			auto _tmp = atom_make(isr);
			tmp.push_back(_tmp);
			if (isr && current && match_data(";")) {
				consume(";", __func__);
			} else if (isr) {
				err_out(PARSE_TIME_ERROR, "Expected ; but got %d at lin %d, col %d, pos %d", current->data.c_str(), lin, col, pos);
			}
		}
		consume("}", __func__);
	} else {
		bool isr = false;
		auto _tmp = atom_make(isr);
		tmp.push_back(_tmp);
		if (isr && current && match_data(";")) {
			consume(";", __func__);
		} else if (isr) {
			err_out(PARSE_TIME_ERROR, "Expected ; but got %s at lin %d, col %d, pos %d", current->data.c_str(), lin, col, pos);
		}
	}
	return new BlockNode(tmp);
}

AST *Parser::make_function_define() {
	crate();
	Token func_name;
	AST* ret_type;
	vector<AST*> vars;
	vector<AST*> templates;
	AST* body;
	++func_cnt;
	if (match_data("function"))
		advance();
	if (!match_type(TT_ID)) {
		cout << "want a name, but get " << current->data << endl;
		exit(-1);
	}
	func_name = *current;
	id_map[func_name.data] = FUNC_ID;
	if (func_name.data.size() > 0)
		add(func_name.data, SK_FUNCTION);
	func_map[func_name.data] = func_cnt;
	advance();
	if (match_data("<")) templates = make_templates();
	consume("(", __func__ );
	while (current && !match_data(")")) {
		vars.push_back(make_var_define());
		if (match_data(")")) break;
		consume(",", __func__ );
	}
	consume(")", __func__ );
	if (!match_type(TT_ID)) {
		ERROR__:
		err_out(PARSE_TIME_ERROR, "Parse time error, want a return type at lin %d, col %d, pos %d", lin, col, pos);
	}
	ret_type = make_type_node();
	check_type(ret_type);
	body = make_block();
	leave();
	return new FunctionNode(func_name, new BlockNode(vars), body, ret_type, templates);
}

AST *Parser::make_if() {
	crate();
	if (match_data("if"))
		advance();
	AST* condition;
	AST* body;
	AST* else_node = nullptr;
	consume("(", __func__ );
	condition = make_or_node();
	consume(")", __func__ );
	body = make_block();
	if (match_data("else"))
		else_node = make_else();
	leave();
	return new IfNode(condition, body, else_node);
}

AST *Parser::make_else() {
	consume("else", __func__ );
	return new ElseNode(make_block());
}

AST *Parser::make_assign(AST* id) {
	consume("=", __func__ );
	auto data = make_expr();
	return new AssignNode(id, data);
}

AST *Parser::make_mem_malloc() {
	consume("new", __func__ );
	AST* object_name = make_type_node();
	if (match_data("(") && !object_name->is_block) {
		auto a = new MemoryMallocNode(make_func_call(object_name));
		if (match_data("[")) {
			cout << "need ';'\n";
			exit(-1);
		}
		return a;
	}
	return new MemoryMallocNode(object_name);
}

AST *Parser::make_class() {
	crate();
	Token name;
	add("this", SK_CLASS);
	int ofst = 0;
	AST* constructor = {};
	vector<AST*>  v;
	vector<AST*> temps;
	if (match_data("class")) advance();
	if (!match_type(TT_ID))
		err_out(PARSE_TIME_ERROR, "want a ID meet '%s', at lin %d, col %d, pos %d", current->data.c_str(), lin, col, pos);
	name = *current;
	IdTypeNode itn;
	itn.rootType = name.data;
	add_var("this", itn);
	ptcr[name.data] = ParseTimeClassRecord();
	id_map[name.data] = CLASS_ID;
	add(name.data, SK_CLASS);
	vector<string> parent_map;
	class_cnt++;
	class_map[name.data] = class_cnt;
	advance();
	if (match_data("<"))
		temps = make_templates();
	if (match_data("(")) {
		advance();
		while (current && !match_data(")")) {
			auto t = want_get(TT_ID);
			parent_map.push_back(t.data);
			advance();
		}
		consume(")", __func__ );
	}
	consume("{", __func__ );
	while (current && !match_data("}")) {
		if (current->data == "constructor") {
		 	constructor = make_function_define();
		}
		AccessStatus as = PRIVATE_MEMBER;
		if (match_data("public")) {
			as = PUBLIC_MEMBER;
			advance();
		} else if (match_data("private")) {
			advance();
		} else if (match_data("protected")) {
			as = PROTECTED_MEMBER;
			advance();
		}
		if (match_data("let")) {
			auto a = make_var_define();
			auto vtype = make_id_type_node(a->children[0]);
			auto vname = a->data.data;
			ptcr[name.data].var_type_record[vname] = vtype;
			scope[scope.size() - 1].var_type_record[vname] = vtype;
			v.push_back(a);
			consume(";", __func__ );
		}
		if (match_data("function")) {
			v.push_back(make_function_define());
		}
	}
	consume("}", __func__ );
	leave();
	auto cls = new ClassNode(name, constructor, parent_map, temps, v);
	
	class_size_record[name.data] = get_class_size(cls);
	cls->class_size = class_size_record[name.data];
	return cls;
}

void Parser::main_parse() {
	while (current) {
		if (match_data("class"))
			result.add(make_class());
		else if (match_data("function"))
			result.add(make_function_define());
		else if (match_data("using")) {
			result.add(make_import_node());
			consume(";", __func__ );
		} else if (match_data("module")) {
			result.add(make_load_module_node());
			consume(";", __func__ );
		} else if (match_data("let")) {
			result.add(make_var_define());
			consume(";", __func__ );
		} else if (match_data("export")) {
			advance();
			if (match_data("function")) {
				auto node = make_function_define();
				(*export_map)[node->data.data] = FUNCTION_EXPORT;
				result.add(node);
			} else if (match_data("let")) {
				auto node = make_var_define();
				string name = node->data.data;
				(*export_map)[node->data.data] = VAR_EXPORT;
				result.add(node);
				consume(";", __func__ );
			} else if (match_data("class")) {
				auto node = make_class();
				(*export_map)[node->data.data] = CLASS_EXPORT;
				result.add(node);
			} else {
				printf("Parse time error, at lin %d, col %d, pos %d, when export\n", lin, col, pos);
				exit(-1);
			}
		}
	}
}

AST* Parser::make_return_node() {
	consume("return", __func__ );
	if (match_data(";")) return new ReturnNode();
	auto node = make_expr();
	return new ReturnNode(node);
}

AST *Parser::make_for() {
	crate();
	AST *init, *end, *change, *body;
	if (match_data("for"))
		advance();
	consume("(", __func__ );
	init = make_var_define();
	consume(";", __func__ );
	end = make_expr();
	consume(";", __func__ );
	change = make_expr();
	consume(")", __func__ );
	body = make_block();
	leave();
	return new ForLoopNode(init, end, change, body);
}

AST *Parser::make_a_assign() {
	auto tmp = make_member_node();
	return make_assign(tmp);
}

AST *Parser::make_while() {
	crate();
	if (match_data("while"))
		advance();
	consume("(", __func__ );
	AST* condition = nullptr;
	AST* body = nullptr;
	condition = make_expr();
	consume(")", __func__ );
	body = make_block();
	leave();
	return new WhileLoopNode(condition, body);
}

AST *Parser::make_inc_node(AST * a) {
	return new IncNode(a);
}

AST *Parser::make_dec_node(AST *a) {
	return new DecNode(a);
}

void Parser::leave() {
	debug_out("Leave scope[%zu]", scope.size());
	if (scope.size() >= 2) {
		scope.pop_back();
	}
}

void Parser::crate() {
	scope.push_back(VarList());
	debug_out("Crate scope[%zu]", scope.size());
}

bool Parser::is_define(string s) {
	for (auto i : scope)
		if (i.is_have(s))
			return true;
	return false;
}

void Parser::add(std::string s, SymbolKind kd) {
	scope[scope.size() - 1].add(s, kd);
}

int Parser::make_class_id() {
	return ++class_cnt;
}

AST *Parser::make_self_add_node(AST *v) {
	if (match_data("+="))
		advance();
	auto expr_ = make_expr();
	return new SelfAdd(v, expr_);
}

AST *Parser::make_self_sub_node(AST *v) {
	if (match_data("-="))
		advance();
	auto expr_ = make_expr();
	return new SelfSub(v, expr_);
}

AST *Parser::make_self_div_node(AST *v) {
	if (match_data("/="))
		advance();
	auto expr_ = make_expr();
	return new SelfDiv(v, expr_);
}

AST *Parser::make_self_mul_node(AST *v) {
	if (match_data("*="))
		advance();
	auto expr_ = make_expr();
	return new SelfMul(v, expr_);
}

AST* Parser::make_case_node() {
	crate();
	consume("case", __func__ );
	auto value = make_expr();
	consume(":", __func__ );
	auto code = make_block();
	leave();
	return new CaseNode(value, code);
}

AST* Parser::make_default_node() {
	crate();
	consume("default", __func__ );
	consume(":", __func__ );
	auto code = make_block();
	leave();
	return new DefaultNode(code);
}

AST* Parser::make_switch_node() {
	vector<AST*> codes;
	consume("switch", __func__ );
	consume("(", __func__ );
	auto id = make_member_node();
	consume(")", __func__ );
	consume("{", __func__ );
	while (current && !match_data("}")) {
		if (match_data("case")) {
			codes.push_back(make_case_node());
		}
		if (match_data("default")) {
			codes.push_back(make_default_node());
		}
	}
	consume("}", __func__ );
	return new SwitchNode(id, codes);
}

AST *Parser::make_continue() {
	if (match_data("continue")) advance();
	return new ContinueNode();
}

AST *Parser::make_break() {
	if (match_data("break")) advance();
	return new BreakNode();
}

AST *Parser::make_list() {
	consume("[", __func__ );
	vector<AST*> values;
	while (current && !match_data("]")) {
		values.push_back(make_expr());
		if (match_data("]")) break;
		consume(",", __func__ );
	}
	consume("]", __func__ );
	return new ListNode(values);
}

AST *Parser::make_get_value_node(AST* id) {
	consume("[", __func__ );
	auto expr = make_expr();
	if (match_data(",")) {
		err_out(PARSE_TIME_ERROR, "need one value", "");
	}
	consume("]", __func__ );
	return new GetValueAtPosition(id, expr);
}

AST *Parser::make_constructor_call_() {
	auto id = make_type_node();
	check_type(id);
	return make_constructor_call(id);
}

AST *Parser::make_constructor_call(AST *name) {
	vector<AST*> args;
	if (match_data("(")) {
		advance();
		while (current && !match_data(")")) {
			args.push_back(make_expr());
			if (match_data(")")) break;
			consume(",", __func__ );
		}
		consume(")", __func__ );
	}
	return new ClassBuildNode(name, args);
}

AST* Parser::make_not_node() {
	consume("!", __func__ );
	auto expr = make_expr();
	return new NotNode(expr);
}

AST *Parser::make_import_node() {
	if (match_data("using"))
		advance();
	if (!match_type(TT_STRING)) {
		printf("Parse time error, at lin %d, col %d, pos %d, want a string\n", lin, col, pos);
		exit(-1);
	}
	auto tok_tmp = *current;
	string path = current->data;
	string buffer, code;
	ifstream ifs(path);
	while (getline(ifs, buffer))
		code += buffer;
	Lexer lexer(code);
	Parser parser(lexer.tokens);
	auto v = parser.result.ast;
	advance();
	return new Import(tok_tmp, v);
}

AST *Parser::make_load_library() {
	consume("load_library", __func__ );
	consume("(", __func__ );
	Token gt = want_get(TT_STRING);
	advance();
	consume(")", __func__ );
	return new LoadLibraryNode(gt);
}

Token Parser::want_get(string s) {
	if (match_type(s)) return *current;
	err_out(PARSE_TIME_ERROR, "want '%s' get '%s', at lin %d, col %d, pos %d",s.c_str() , current->data.c_str(), lin, col, pos);
	return Token();
}

AST *Parser::make_load_module_node() {
	if (match_data("module")) advance();
	auto name = make_member_node();
	string name_str = name->data.data;
	add(name_str, SK_MODULE);
	consume("=", __func__ );
	auto mod = make_load_library();
	return new LoadModuleNode(name, mod);
}

int Parser::get_class_size(AST *a) {
	auto si = a->children.size();
	for (auto i : a->parents)
		si += class_size_record[i];
	class_size_record[a->data.data] = si;
	return si;
}

AST *Parser::make_lambda() {
	vector<AST*> value;
	AST* body;
	if (match_data("lambda"))
		advance();
	consume("(", __func__ );
	while (current && !match_data(")")) {
		value.push_back(make_var_define());
		if (match_data(")")) break;
		consume(",", __func__ );
	}
	consume(")", __func__ );
	body = make_block();
	return new LambdaNode(value, body);
}

void Parser::check_type(AST *a) {
	string type = a->type;
	if (type != AST_CLASS && type != AST_TYPE) {
		err_out(PARSE_TIME_ERROR, "want a type", 0);
	}
	if (type == AST_CLASS) {
		if (class_map[a->data.data] == 0) {
			err_out(PARSE_TIME_ERROR, "class not exist", 0);
		}
		return;
	}
	if (type == AST_TYPE) {
		string root_type = a->data.data;
		if (class_map[root_type] == 0) {
			err_out(PARSE_TIME_ERROR, "class not exist", 0);
		}
		if (!a->children.empty()) {
			for (auto i : a->children)
				check_type(i);
		}
		return;
	}
	err_out(PARSE_TIME_ERROR, "want a type", 0);
}

bool Parser::is_type(AST *a) {
	string type = a->type;
	if (type != AST_CLASS && type != AST_TYPE)
		return false;
	if (type == AST_CLASS) {
		if (class_map[a->data.data] == 0)
			return false;
		return true;
	}
	if (type == AST_TYPE) {
		string root_type = a->data.data;
		if (class_map[root_type] == 0)
			return false;
		if (!a->children.empty())
			for (auto i : a->children)
				check_type(i);
		return true;
	}
	return false;
}

#endif