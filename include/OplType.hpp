#ifndef OPP
#define OPP
#include <vector>
#include <iostream>
#include <cmath>
#include <unordered_map>
#include <map>
using namespace std;
#define IS_WORD(x) (('a' <= (x) && (x) <= 'z') || ('A' <= (x) && (x) <= 'Z') || (x) == '_')

class IdTypeNode {
public:
	string rootType;
	vector<IdTypeNode> templateType;
};

void prd(int dep) {
	while (dep--) cout << "   ";
}

void print_type_node(const IdTypeNode& itn, int dep = 1) {
	prd(dep-1);
	cout << "ROOT_TYPE = " << itn.rootType << " \n";
	if (!itn.templateType.empty()) {
		for (auto i : itn.templateType)
			print_type_node(i, dep + 1);
	}
}

string hex_to_dec(string hexStr) {
	unsigned long long decimal = 0;
	for (char c : hexStr) {
		int val;
		if (c >= '0' && c <= '9') val = c - '0';
		else if (c >= 'A' && c <= 'F') val = 10 + (c - 'A');
		else if (c >= 'a' && c <= 'f') val = 10 + (c - 'a');
		else {
			cerr << "Error in hex to dec " << c << endl;
			return "0";
		}
		decimal = decimal * 16 + val;
	}
	return to_string(decimal);
}

string bin_to_dec(string x) {
	unsigned long long num = 0;
	for (char c : x) {
		num = num * 2 + (c - '0');
	}
	return to_string(num);
}

string oct_to_dec(string x) {
	unsigned long long num = 0;
	for (char c : x) {
		num = num * 8 + (c - '0');
	}
	return to_string(num);
}
enum SymbolKind {
	SK_FUNCTION,
	SK_CLASS,
	SK_VAR,
	SK_MODULE,
	SK_LIST
};

const std::string TT_INTEGER    = "TT_INTEGER";
const std::string TT_DOUBLE     = "TT_DOUBLE";
const std::string TT_NEW_LINE   = "TT_NEW_LINE";
const std::string TT_STRING     = "TT_STRING";
const std::string TT_BOOL       = "TT_BOOL";
const std::string TT_MOD	    = "TT_MOD";
const std::string TT_SELF_MOD	= "TT_MOD";
const std::string TT_NOT        = "TT_NOT";
const std::string TT_NOT_EQ     = "TT_NOT_EQ";
const std::string TT_ID         = "TT_ID";
const std::string TT_LBRACKET_S = "TT_LBRACKET_S";
const std::string TT_KEY        = "TT_KEY";
const std::string TT_PLUS       = "TT_PLUS";
const std::string TT_SUB        = "TT_SUB";
const std::string TT_MUL        = "TT_MUL";
const std::string TT_DIV        = "TT_DIV";
const std::string TT_POW        = "TT_POW";
const std::string TT_RBRACKET_S = "TT_RBRACKET_S";
const std::string TT_LBRACKET_M = "TT_LBRACKET_M";
const std::string TT_RBRACKET_M = "TT_RBRACKET_M";
const std::string TT_LBRACKET_B = "TT_LBRACKET_B";
const std::string TT_RBRACKET_B = "TT_RBRACKET_B";
const std::string TT_PERIOD     = "TT_PERIOD";
const std::string TT_SEMICOLON  = "TT_SEMICOLON";
const std::string TT_EQ         = "TT_EQ";
const std::string TT_BIGER      = "TT_BIGER";
const std::string TT_SMALLER    = "TT_SMALLER";
const std::string TT_BRE        = "TT_BRE";
const std::string TT_SRE        = "TT_SRE";
const std::string TT_LM         = "TT_LM";
const std::string TT_AND 		= "TT_AND";
const std::string TT_BIT_AND    = "TT_BIT_AND";
const std::string TT_OR 		= "TT_OR";
const std::string TT_LEFT_MOVE  = "TT_LEFT_MOVE";
const std::string TT_RIGHT_MOVE = "TT_RIGHT_MOVE";
const std::string TT_BIT_OR 	= "TT_BIT_OR";
const std::string TT_RM         = "TT_RM";
const std::string TT_SLM        = "TT_SLM";
const std::string TT_SRM        = "TT_SRM";
const std::string TT_COLON      = "TT_COLON";
const std::string TT_COMMA      = "TT_COMMA";
const std::string TT_ASSIGN     = "TT_ASSIGN";
const std::string TT_SELFADD    = "TT_SELFADD";
const std::string TT_SELFSUB    = "TT_SELFSUB";
const std::string TT_SELFMUL    = "TT_SELFMUL";
const std::string TT_SELFDIV    = "TT_SELFDIV";

class Token {
public:
	std::string type; std::string data;
	void debug();
};

void Token::debug()  {
	cout << type << " : '" << data << "'" << endl;
}

enum AccessStatus {
	PUBLIC_MEMBER, PRIVATE_MEMBER, PROTECTED_MEMBER
};
const std::string AST_MEMBER   = "MEMBER";
const std::string AST_ID       = "ID";
const std::string AST_BIN_OP   = "BIN_OP";
const std::string AST_DIGIT    = "DIGIT";
const std::string AST_STRING   = "STRING";
const std::string AST_VAR_DEF  = "VAR_DEFINE";
const std::string AST_FUNC_CALL= "FUNC_CALL";
const std::string AST_FUNCTION_DEFINE = "FUNC_DEFINE";
const std::string AST_BLOCK    = "BLOCK";
const std::string AST_IF       = "IF";
const std::string AST_ELSE     = "ELSE";
const std::string AST_ASSIGN   = "ASSIGN";
const std::string AST_MEM_MALLOC = "MEM_MALLOC";
const std::string AST_CLASS    = "CLASS_NODE";
const std::string AST_RETURN   = "RETURN";
const std::string AST_FOR_LOOP = "FOR_LOOP";
const std::string AST_WHILE_LOOP = "WHILE_LOOP";
const std::string AST_INC        = "SELF_INC";
const std::string AST_CONV			 = "TYPE_CONVERT";
const std::string AST_DEC        = "SELF_DEC";
const std::string AST_SELF_ADD   = "SELF_ADD";
const std::string AST_SELF_SUB   = "SELF_SUB";
const std::string AST_SELF_DIV   = "SELF_DIV";
const std::string AST_SELF_MUL   = "SELF_MUL";
const std::string AST_BREAK      = "AST_BREAK";
const std::string AST_LAMBDA     = "AST_LAMBDA";
const std::string AST_CONTINUE   = "AST_CONTINUE";
const std::string AST_LIST       = "AST_LIST";
const std::string AST_GET_VALUE  = "AST_GET_VALUE";
const std::string AST_TYPE       = "TYPE_NODE";
const std::string AST_CONSTRUCTOR_CALL = "CONSTRUCTOR_CALL";
const std::string AST_NOT        = "NOT";
const std::string AST_IMPORT     = "IMPORT";
const std::string AST_LOAD_LIB   = "LOAD_LIB";
const std::string AST_MODULE_LOAD = "MODULE_LOAD";
const std::string AST_SWITCH = "AST_SWITCH";
const std::string AST_CASE = "AST_CASE";

const std::string AST_DEFAULT   = "AST_DEFAULT";

enum SymbolId {
	VAR_ID,
	FUNC_ID,
	CLASS_ID
};

enum ExportId {
	FUNCTION_EXPORT,
	CLASS_EXPORT,
	VAR_EXPORT
};

class ParseTimeClassRecord {
public:
	map<string, IdTypeNode> var_type_record;
	string get_child_type(string, int);
	string get_root_type(string);
};

string ParseTimeClassRecord::get_root_type(string s) {
	if (var_type_record.find(s) == var_type_record.end()) {
		return {};
	}
	string tmp = var_type_record[s].rootType;
	return tmp;
}

string ParseTimeClassRecord::get_child_type(string s, int i) {
	if (var_type_record.find(s) == var_type_record.end()) {
		return {};
	}
	return var_type_record[s].templateType[i].rootType;
}

class AST {
public:
	AST() {
		offset = 0;
		id_node = nullptr;
		as = PUBLIC_MEMBER;
		class_size = 0;
	}
	IdTypeNode idt;
	int class_size, offset;
	int member_offset;
	string parent_name;
	AccessStatus as;
	vector<AST*> children;
	vector<string> parents;
	vector<vector<AST*>> v_ast;
	AST* id_node;
	Token data;
	std::string type;
};

void set_offset(AST* a, int& of) {
	a->offset = of++;
}

class DefaultNode : public AST {
public:
	DefaultNode(AST* a) {
		children.push_back(a);
		type = AST_DEFAULT;
	}
};

class CaseNode : public AST {
public:
	CaseNode(AST* value, AST* code) {
		type = AST_CASE;
		children.push_back(value);
		children.push_back(code);
	}
};

class SwitchNode : public AST {
public:
	SwitchNode(AST* name, vector<AST*> a) {
		type = AST_SWITCH;
		children.push_back(name);
		for (auto i : a)
			children.push_back(i);
	}
};

class GetValueAtPosition : public AST {
public:
	GetValueAtPosition(AST* list_name, AST* expr) {
		type = AST_GET_VALUE;
		children.push_back(list_name);
		children.push_back(expr);
	}
};

class Program {
public:
	Program() {}
	AST*& operator[](int);
	int size();
	void add(AST*);
	vector<AST*> ast;
	unordered_map<string, ExportId> export_map;
	unordered_map<string, Program*> import_map;
};

AST*& Program::operator[](int pos) { return ast[pos]; }

int Program::size() { return ast.size(); }

void Program::add(AST *a) { ast.push_back(a); }

class BlockNode : public AST {
public:
	BlockNode(vector<AST*> v) { type = AST_BLOCK; children = v; }
};

class IncNode : public AST {
public:
	IncNode(AST* name) {
		type = AST_INC;
		children.push_back(name);
	}
};

class NotNode : public AST {
public:
	NotNode(AST* condition) {
		children.push_back(condition);
		type = AST_NOT;
	}
};

class DecNode : public AST {
public:
	DecNode(AST* name) {
		type = AST_DEC;
		children.push_back(name);
	}
};

class SelfAdd : public AST {
public:
	SelfAdd(AST* var, AST* value) {
		type = AST_SELF_ADD;
		children.push_back(var);
		children.push_back(value);
	}
};

class SelfSub : public AST {
public:
	SelfSub(AST* var, AST* value) {
		type = AST_SELF_SUB;
		children.push_back(var);
		children.push_back(value);
	}
};

class Import : public AST {
public:
	Import(Token file_path, vector<AST*> file_data) {
		type = AST_IMPORT;
		data = file_path;
		children = file_data;
	}
};

class SelfMul : public AST {
public:
	SelfMul(AST* var, AST* value) {
		type = AST_SELF_MUL;
		children.push_back(var);
		children.push_back(value);
	}
};

class TypeNode : public AST {
public:
	TypeNode(Token name, vector<AST*> template_list) {
		type = AST_TYPE;
		data = name;
		children = template_list;
	}
};

IdTypeNode make_id_type_node(AST* node) {
	IdTypeNode itn;
	itn.rootType = node->data.data;
	if (!node->children.empty()) {
		for (auto i : node->children) {
			itn.templateType.push_back(make_id_type_node(i));
		}
	}
	print_type_node(itn);
	return itn;
}

class SelfDiv : public AST {
public:
	SelfDiv(AST* var, AST* value) {
		type = AST_SELF_DIV;
		children.push_back(var);
		children.push_back(value);
	}
};

class ClassBuildNode : public AST {
public:
	ClassBuildNode(AST* name, vector<AST*> args) {
		type = AST_CONSTRUCTOR_CALL;
		children.push_back(name);
		for (auto i : args)
			children.push_back(i);
	}
};

class ReturnNode : public AST {
public:
	ReturnNode(AST* data = nullptr) {
		type = AST_RETURN;
		children.push_back(data);
	}
};

class ClassNode : public AST {
public:
	ClassNode(Token class_name, AST* constructor, vector<AST*> v) {
		type = AST_CLASS;
		data = class_name;
		id_node = constructor;
		children = v;
	}
	ClassNode(Token class_name, AST* constructor, vector<string> parents, vector<AST*> templates, vector<AST*> member) {
		type = AST_CLASS;
		data = class_name;
		id_node = constructor;
		v_ast.push_back(templates);
		children = member;
		this->parents = parents;
	}
	ClassNode(Token class_name, AST* constructor, vector<AST*> templates, vector<AST*> member) {
		type = AST_CLASS;
		data = class_name;
		id_node = constructor;
		v_ast.push_back(templates);
		children = member;
	}
};

class IfNode : public AST {
public:
	IfNode(AST* condition, AST* body, AST* else_node = nullptr) {
		type = AST_IF;
		children.push_back(condition);
		children.push_back(body);
		children.push_back(else_node);
	}
};

class ListNode : public AST {
public:
	ListNode(vector<AST*> value) {
		children = value;
		type = AST_LIST;
	}
};

class MemoryMallocNode : public AST {
public:
	MemoryMallocNode(AST* a) {
		type = AST_MEM_MALLOC;
		children.push_back(a);
	}
};

class ElseNode : public AST {
public:
	ElseNode(AST* a) {
		type = AST_ELSE;
		children.push_back(a);
	}
};

class AssignNode : public AST {
public:
	AssignNode(AST* id, AST* value) {
		type = AST_ASSIGN;
		children.push_back(id);
		children.push_back(value);
	}
};

class FunctionNode : public AST {
public:
	FunctionNode(Token func_name, AST* value_list, AST* body, AST* return_type, AccessStatus as = PUBLIC_MEMBER) {
		type = AST_FUNCTION_DEFINE;
		data = func_name;
		this->as = as;
		children.push_back(value_list);
		children.push_back(body);
		children.push_back(return_type);
	}
	FunctionNode(Token func_name, AST* value_list, AST* body, AST* return_type, const vector<AST*>& templates, AccessStatus as = PUBLIC_MEMBER) {
		type = AST_FUNCTION_DEFINE;
		data = func_name;
		this->as = as;
		children.push_back(value_list);
		children.push_back(body);
		children.push_back(return_type);
		v_ast.push_back(templates);
	}
};

class CastNode : public AST {
public:
	CastNode(AST* _type, AST* expression) {
		type = AST_CONV;
		children.push_back(_type);
		children.push_back(expression);
	}
};

class VarDefineNode : public AST {
public:
	VarDefineNode(Token var_name, AST* var_type = nullptr, AST* var_data = nullptr) {
		type = AST_VAR_DEF;
		data = var_name;
		children.push_back(var_type);
		children.push_back(var_data);
	}
};

class IdNode : public AST {
public:
	IdNode(Token tok) {
		type = AST_ID;
		data = tok;
	}
	IdNode(Token tok, vector<AST*> t) {
		type = AST_ID;
		data = tok;
		children = t;
	}
};

class LambdaNode : public AST {
public:
	LambdaNode(vector<AST*> value, AST* code) {
		v_ast.push_back(value);
		children.push_back(code);
		type = AST_LAMBDA;
	}
};

class DigitNode : public AST {
public:
	DigitNode(Token num) {
		type = AST_DIGIT;
		data = num;
	}
};

class StringNode : public AST {
public:
	StringNode(Token str) {
		type = AST_STRING;
		data = str;
	}
};

class FunctionCallNode : public AST {
public:
	FunctionCallNode(AST* func_name, vector<AST*> func_value_list) {
		children.push_back(func_name);
		for (auto i : func_value_list)
			children.push_back(i);
		type = AST_FUNC_CALL;
	}
	FunctionCallNode(AST* func_name, vector<AST*> func_value_list, vector<AST*> templates) {
		children.push_back(func_name);
		for (auto i : func_value_list)
			children.push_back(i);
		type = AST_FUNC_CALL;
		v_ast.push_back(templates);
	}
};

class ForLoopNode : public AST {
public:
	ForLoopNode(AST* init, AST* end, AST* change, AST* body) {
		type = AST_FOR_LOOP;
		children.push_back(init);
		children.push_back(end);
		children.push_back(change);
		children.push_back(body);
	}
};

class ContinueNode : public AST {
public:
	ContinueNode() {
		type = AST_CONTINUE;
	}
};

class BreakNode : public AST {
public:
	BreakNode() {
		type = AST_BREAK;
	}
};

class WhileLoopNode : public AST {
public:
	WhileLoopNode(AST* condition, AST* body) {
		type = AST_WHILE_LOOP;
		children.push_back(condition);
		children.push_back(body);
	}
};

class LoadModuleNode : public AST {
public:
	LoadModuleNode(AST* id, AST* load_node) {
		type = AST_MODULE_LOAD;
		children.push_back(id);
		children.push_back(load_node);
	}
};

class LoadLibraryNode : public AST {
public:
	LoadLibraryNode(Token id) {
		type = AST_LOAD_LIB;
		data = id;
	}
};

class MemberNode : public AST {
public:
	MemberNode(AST* base, AST* member) {
		type = AST_MEMBER;
		children.push_back(base); // children[0]
		children.push_back(member); // children[1]
	}
};

class BinOpNode : public AST {
public:
	BinOpNode(Token op, AST* left, AST* right) {
		data = op;
		type = AST_BIN_OP;
		children.push_back(left);
		children.push_back(right);
	}
};

void pd(int dp) {
	while (dp--) printf("  ");
}

void pt(AST* a) {
	cout << "<" << a->type << ": '" << a->data.data << "'> ";
}

// 添加颜色控制宏定义（兼容多数终端）
#define COLOR_RESET  string("\033[0m")
#define COLOR_GRAY   string("\033[90m")
#define COLOR_RED    string("\033[31m")
#define COLOR_GREEN  string("\033[32m")
#define COLOR_YELLOW string("\033[33m")
#define COLOR_BLUE   string("\033[34m")

void print_tree(AST* a, int depth = 0, bool is_last = true, vector<bool> indents = {}) {
	if (!a) return;
	
	string prefix;
	for (size_t i = 0; i < indents.size(); ++i) {
		// prefix += is_last ? "|   " : "    ";
		prefix += "|   ";
	}
	prefix += is_last ? "+---" : "|---";
	
	string type_color = a->type == AST_MEMBER ? COLOR_GREEN : COLOR_BLUE;
	
	string node_info = type_color + "<" + a->type + COLOR_RESET;
	
	if (!a->parent_name.empty()) {
		node_info += COLOR_GRAY + " parent:" + COLOR_RESET + "'" + COLOR_YELLOW + a->parent_name + COLOR_RESET + "'";
	}
	
	if (!a->data.data.empty()) {
		node_info += COLOR_GRAY + " data:" + COLOR_RESET + "'" + COLOR_YELLOW + a->data.data + COLOR_RESET + "'";
	}
	
	if (a->offset != 0 || a->class_size != 0) {
		node_info += COLOR_GRAY + " [" + COLOR_RESET;
		if (a->offset != 0) node_info += COLOR_RED + "offset=" + to_string(a->offset) + COLOR_RESET;
		if (a->class_size != 0) node_info += (a->offset != 0 ? " " : "") + COLOR_RED + "size=" + to_string(a->class_size) + COLOR_RESET;
		node_info += COLOR_GRAY + "]" + COLOR_RESET;
	}
	node_info += ">";
	
	cout << COLOR_GRAY << prefix << COLOR_RESET << node_info << endl;
	
	vector<bool> new_indents(indents);
	new_indents.push_back(!is_last);
	
	if (a->type == AST_MEMBER) {
		cout << COLOR_GRAY << prefix << "    |--- [Base]" << COLOR_RESET << endl;
		print_tree(a->children[0], depth + 1, false, new_indents);
		cout << COLOR_GRAY << prefix << "    +--- [Member]" << COLOR_RESET << endl;
		print_tree(a->children[1], depth + 1, true, new_indents);
		return;
	}
	
	for (size_t i = 0; i < a->children.size(); ++i) {
		bool last_child = (i == a->children.size() - 1);
		print_tree(a->children[i], depth + 1, last_child, new_indents);
	}
	
	if (!a->v_ast.empty()) {
		string tmpl_prefix = prefix + (new_indents.empty() ? "" : "    ");
		cout << COLOR_GRAY << tmpl_prefix << "+--- <Templates>" << COLOR_RESET << endl;
		
		vector<bool> tmpl_indents(new_indents);
		tmpl_indents.push_back(false);
		
		for (auto& tlist : a->v_ast) {
			for (size_t i = 0; i < tlist.size(); ++i) {
				print_tree(tlist[i], depth + 2, i == tlist.size()-1, tmpl_indents);
			}
		}
	}
}


enum BUILDIN_TYPE {
	VOID, INTEGER, DOUBLE, STRING, CHAR, BOOL, LIST
};

vector<std::string> keys = {
		"if", "else", "for", "while", "class",
		"function", "let", "const", "new",
		"public", "private", "protected", "return",
		"continue", "break", "using", "module",
		"switch", "case", "default", "export",
		"lambda"
};

#endif 