#ifndef C_PROGRAM_LANGUAGE_COMPILER_HPP
#define C_PROGRAM_LANGUAGE_COMPILER_HPP
#include "OplType.hpp"
#include "color256.hpp"
#include <unordered_map>

enum NameType {
	NAME_ID, NAME_DIGIT, NAME_STRING
};

struct Name {
	NameType type;
	string label, name;
	string class_name;
	int offset;
	Name() { type = NAME_ID; offset = -1; }
	Name(string name) {
		type = NAME_ID;
		offset = -1;
		this->name = name;
	}
	Name(string label, string name) {
		type = NAME_ID;
		offset = -1;
		this->label = label;
		this->name = name;
	}
};

#define CONS const string
CONS OPER_ADD  = "ADD";
CONS OPER_OR   = "OR";
CONS CLASS_END = "CLASS_END";
CONS CLASS_BEG = "CLASS_BEGIN";
CONS OPER_NOP  = "NOP";
CONS OPER_SPACE = "NEW_SPACE";
CONS FUNC_BEG  = "FUNC_BEGIN";
CONS OPER_GET_RET_VAL = "GET_RETURN_VALUE";
CONS FUNC_END  = "FUNC_END";
CONS OPER_AND  = "AND";
CONS OPER_NOT  = "NOT";
CONS OPER_SUB  = "SUB";
CONS OPER_TYPE_CAST = "TYPE_CAST";
CONS OPER_LOAD_LIB  = "LOAD_LIBRARY";
CONS OPER_SET  = "SET";
CONS OPER_MUL  = "MUL";
CONS OPER_DIV  = "DIV";
CONS OPER_ALLOC= "ALLOC";
CONS OPER_LEAVE = "LEAVE";
CONS OPER_LD_RET = "LOAD_RETURN_VALUE";
CONS OPER_MOD  = "MOD";
CONS OPER_IF   = "IF";
CONS OPER_GOTO = "GOTO";
CONS OPER_CALL = "CALL";
CONS OPER_CAST = "CAST";
CONS OPER_VAR  = "VAR_DEF";
CONS OPER_ASSI = "VAR_ASSIGN";
CONS OPER_LIST = "NEW_LIST";
CONS OPER_OFFSET = "OFFSET";
CONS OPER_LOAD = "LOAD";
CONS OPER_NEW  = "NEW";
CONS OPER_DEL  = "DEL";
CONS OPER_RET  = "RET";
CONS OPER_PARAM = "PARAM";

struct OperatorCommand {
  string label_name;
  vector<string> codes;
  OperatorCommand(string label_name, vector<string> opc) {
	  this->label_name = label_name;
	  this->codes = opc;
  }
  OperatorCommand(vector<string> opc) {
	  codes = opc;
	  label_name = "<DEFAULT_LABEL>";
  }
};

#define PName Name*
#define RENUL return nullptr;

class FunctionObjectInfo {
public:
	string function_name;
	int function_ret_type;
	int father_class;
	int access_status;
	int value_size;
	
	FunctionObjectInfo() {
		function_name = "<UNKNOW_FUNCTION>";
		function_ret_type = VOID;
		father_class = -1;
		access_status = PUBLIC_MEMBER;
		value_size = 0;
	}
};

class ClassValueNode {
public:
	string name;
	int id, type;
	int access_status;
	ClassValueNode() {
		type = VOID;
		access_status = PUBLIC_MEMBER;
		name = "<UNKNOWN_VAR>";
	}
};

class ObjectInfo {
public:
	int object_id;
	int member_offset;
	string object_name;
	vector<int> parents_point;
	unordered_map<string, int> member_offset_record;
	unordered_map<string, FunctionObjectInfo> funcs;
	unordered_map<string, ClassValueNode> values;
	void add_offset(string, int);
	int& operator[](string);
	ObjectInfo() { }
	ObjectInfo(int id, string object_name) {
		member_offset = 0;
		this->object_id = id;
		this->object_name = object_name;
	}
	ObjectInfo(string object_name) {
		member_offset = 0;
		this->object_name = object_name;
	}
	void add_function(FunctionObjectInfo);
	void add_value(ClassValueNode);
	bool is_have(string);
	bool is_have_func(string);
	int get(string);
	bool is_have_value(string);
};

void ObjectInfo::add_offset(string mem_name, int offset) {
	member_offset_record[mem_name] = offset;
}

int& ObjectInfo::operator[](string mem_name) {
	auto it = member_offset_record.find(mem_name);
	return it->second;
}

int ObjectInfo::get(std::string s) {
	return member_offset_record[s];
}

bool ObjectInfo::is_have_func(std::string fn) {
	return funcs.find(fn) != funcs.end();
}

bool ObjectInfo::is_have_value(std::string val) {
	return values.find(val) != values.end();
}

bool ObjectInfo::is_have(std::string n) {
	return funcs.find(n) != funcs.end() || values.find(n) != values.end();
}

void ObjectInfo::add_function(FunctionObjectInfo foi) {
	funcs[foi.function_name] = foi;
	member_offset_record[foi.function_name] = ++member_offset;
}

void ObjectInfo::add_value(ClassValueNode cvn) {
	values[cvn.name] = cvn;
	member_offset_record[cvn.name] = ++member_offset;
}

class CompileResult {
public:
	unordered_map<string, CompileResult*> imports;
	unordered_map<string, int> export_map;
	vector<OperatorCommand> opcs;
	void push_back(OperatorCommand);
	int get_export(string);
	int size();
	void operator+(CompileResult);
};

int CompileResult::size() {
	return opcs.size();
}

int CompileResult::get_export(string id_) {
	auto i = export_map.find(id_);
	if (i != export_map.end())
		return i->second;
	cout << "member '" << id_ << "' not export\n";
	exit(-1);
}

void CompileResult::push_back(OperatorCommand opcd) {
	opcs.push_back(opcd);
}

void CompileResult::operator+(CompileResult c) {
	for (auto & import : c.imports)
		this->imports[import.first] = import.second;
	for (auto & i : c.export_map)
		this->export_map[i.first] = i.second;
	for (auto i : c.opcs) this->opcs.push_back(i);
}

class Compiler {
public:
	Compiler(vector<AST*> id) {
		debug_out("Compile start.", "");
		this->id_node = id;
		init();
		start_compile();
	}
	void init();
	CompileResult get_compile_result();
private:
	int class_offset, class_count;
	unordered_map<string, ObjectInfo> objects;
	unordered_map<string, FunctionObjectInfo> funcs;
	unordered_map<string, int> class_map;
	unordered_map<string, int> class_size_map;
	unordered_map<string, string> var_type_map;
	unordered_map<string, string> this_rec;
	int get_offset(string, string);
	// vector<OperatorCommand> opcs;
	CompileResult opcs;
	int var_cnt, label_cnt, lb_c, le_c, lc, elc, th_c;
	vector<AST*> id_node;
	string make_label();
	string make_var_name();
	string make_case_label();
	string make_this_name(string);
	string make_else_label();
	string make_loop_begin_label();
	string make_loop_end_label();
	void register_var(string, string);
	void start_compile();
	int get_ast_type(AST*);
	void add_value(string, string);
	void add_function(string, string);
	void consume(AST*, string, string);
	bool match_type(AST*, string);
	PName visit_load_library_node(AST*);
	PName visit_atom(AST*, string, string);
	PName visit_function_define_node(AST*);
	PName visit_for_loop_node(AST*); // for(;;) { ... }
	PName visit_while_loop_node(AST*); // while(...) { ... }
	PName visit_bin_op_node(AST*); // BinOp
	PName visit_self_add_node(AST*); // x++, ++x
	PName visit_type_cast_node(AST*); // (type)(expr)
	PName visit_if_node(AST*, string, string);
	PName visit_continue_node(string, string);
	PName visit_break_node(string, string);
	PName visit_switch_node(AST*);
	PName visit_digit_node(AST*);
	PName visit_self_sub_node(AST*); // x--, --x
	PName visit_case_node(vector<AST*>, vector<string>, string);
	PName visit_make_not_node(AST*);
	PName visit_list_node(AST*); // [1,2,3]
	PName visit_id_node(AST*);
	PName visit_class_node(AST*);
	PName visit_added_by(AST*); // x += y
	PName visit_subbed_by(AST*); // x -= y
	PName visit_member_node(AST*); // className.memberName.functionName( ... )
	PName visit_pos_value(AST*); // listName[position]
	PName visit_self_div_node(AST*); // x /= y
	PName visit_self_mul_node(AST*); // x *= y
	PName visit_self_mod_node(AST*); // x %= y
	PName visit_mem_malloc_node(AST*); // new ClassName<TypeList>(Args)
	PName visit_ret_node(AST*); // return <EXPR>
	PName visit_call_node(AST*); // funcName(args)
	PName visit_var_def_node(AST*); // let NAME: TYPE = VALUE
	PName visit_var_assign_node(AST*); // NAME = VALUE
	PName visit_string_node(AST*);
};

PName Compiler::visit_load_library_node(AST* a) {
	// using "name";
	Compiler compiler(a->children);
	auto temp = compiler.opcs;
}

void Compiler::init() {
	normal_debug();
	class_size_map["int"] =
		 class_size_map["double"] =
			 class_size_map["string"] =
				class_size_map["list"] =
					class_size_map["bool"] =
						class_size_map["void"] = 1;
	lc = 0;
	label_cnt = 0;
	lb_c = le_c = 0;
	elc = 0;
	var_cnt = 0;
	class_count = 0;
	class_offset = 0;
	th_c = 0;
}

void Compiler::register_var(string a, string type__) {
	var_type_map[a] = type__;
}

void Compiler::start_compile() {
	normal_debug();
	debug_out("Compile start. size = %zu", id_node.size());
	for (int i = 0; i < id_node.size(); ++i) {
		if (id_node[i]) {
			visit_atom(id_node[i], "<ERROR>", "<ERROR>");
		}
	}
}

CompileResult Compiler::get_compile_result() {
	normal_debug();
	return this->opcs;
}

string Compiler::make_this_name(string name) {
	normal_debug();
	return "__" + name + "_this" + to_string(++th_c);
}

int Compiler::get_ast_type(AST *a) {
	normal_debug();
	string s = a->data.data;
	return class_map[s];
}

PName Compiler::visit_function_define_node(AST* a) {
	normal_debug();
	string func_name_ = a->data.data;
	var_type_map[func_name_] = "FUNCTION";
	opcs.push_back(OperatorCommand(make_label(), {FUNC_BEG, func_name_}));
	auto value_list = a->children[0]->children;
	auto func_body  = a->children[1];
	for (int i = 0; i < value_list.size(); ++i) {
		string id = "#" + to_string(i);
		string va = value_list[i]->data.data;
		opcs.push_back(OperatorCommand(make_label(), {OPER_LOAD, va, id}));
	}
	visit_atom(func_body, "", "");
	opcs.push_back(OperatorCommand(make_label(), {FUNC_END}));
	return new Name(func_name_);
}

string Compiler::make_else_label() {
	normal_debug();
	return "else" + to_string(elc++);
}

PName Compiler::visit_if_node(AST *a, std::string lb, std::string le) {
	normal_debug();
	string else_node = make_else_label();
	string exit_label = make_label();
	string condition = visit_bin_op_node(a->children[0])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_IF, OPER_NOT, condition, OPER_GOTO, else_node}));
	visit_atom(a->children[1], lb, le);
	opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, exit_label}));
	if (!a->children[2]) {
		opcs.push_back(OperatorCommand(else_node, {OPER_NOP}));
		opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, exit_label}));
	} else {
		opcs.push_back(OperatorCommand(else_node, {OPER_NOP}));
		auto el_node = a->children[2];
		visit_atom(el_node->children[0], lb, le);
		opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, exit_label}));
	}
	opcs.push_back(OperatorCommand(exit_label, {OPER_NOP}));
	RENUL;
}

PName Compiler::visit_string_node(AST *a) {
	normal_debug();
	string vn = make_var_name();
	opcs.push_back(OperatorCommand(make_label(), {vn, "=", a->data.data}));
	return new Name(vn);
}

PName Compiler::visit_digit_node(AST *a) {
	normal_debug();
	string vn = make_var_name();
	opcs.push_back(OperatorCommand(make_label(), {vn, "=", a->data.data}));
	return new Name(vn);
}

PName Compiler::visit_case_node(vector<AST*> a, vector<string> labels, string end) {
	normal_debug();
	if (a.size() != labels.size()) {
		printf("CompileTimeError: a.size() = %zu, labels.size() = %zu\n", a.size(), labels.size());
		exit(-1);
	}
	for (int i = 0; i < a.size(); ++i) {
		opcs.push_back(OperatorCommand(labels[i], {OPER_NOP}));
		if (a[i]->type == AST_CASE) visit_atom(a[i]->children[1], "", end);
		else if (a[i]->type == AST_DEFAULT) {
			visit_atom(a[i]->children[0], "", end);
		} else {
			printf("CompileTimeError: UnKnow operator command\n");
			exit(-1);
		}
	}
	opcs.push_back(OperatorCommand(end, {OPER_NOP}));
	RENUL;
}

string Compiler::make_loop_begin_label() {
	normal_debug();
	return "lb" + to_string(lb_c++);
}

string Compiler::make_loop_end_label() {
	normal_debug();
	return "le" + to_string(le_c++);
}

string Compiler::make_case_label() {
	normal_debug();
	return "case" + to_string(lc++);
}

PName Compiler::visit_break_node(std::string begin, std::string end) {
	normal_debug();
	opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, end}));
	RENUL;
}

PName Compiler::visit_continue_node(std::string begin, std::string end) {
	normal_debug();
	opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, begin}));
	RENUL;
}

PName Compiler::visit_id_node(AST *a) {
	normal_debug();
	if (a->type == AST_GET_VALUE)
		return visit_pos_value(a);
	auto res = new Name(a->data.data);
	string name = a->data.data;
	if (var_type_map.find(name) != var_type_map.end()) {
		res->offset = objects[var_type_map[a->data.data]].get(name);
		res->class_name = var_type_map[a->data.data];
	}
	return res;
}

void Compiler::add_value(string __class_name, string vn) {
	normal_debug();
	ClassValueNode cvn;
	cvn.name = vn;
	this->objects[__class_name].add_value(cvn);
}

void Compiler::add_function(string __class_name, string vn) {
	normal_debug();
	FunctionObjectInfo foi;
	foi.function_name = vn;
	this->objects[__class_name].add_function(foi);
}

int Compiler::get_offset(string __class_name, string member_name) {
	normal_debug();
	return this->objects[__class_name].get(member_name);
}

bool Compiler::match_type(AST* tre, string str) {
	normal_debug();
	if (!tre) return false;
	auto condition = tre->type == str;
	return condition;
}

PName Compiler::visit_switch_node(AST* a) {
	normal_debug();
	string name = visit_bin_op_node(a->children[0])->name;
	string end_l = make_loop_end_label();
	std::vector<string> case_label;
	std::vector<AST*> cases;
	for (int i = 1; i < a->children.size(); ++i) {
		case_label.push_back(make_case_label());
		cases.push_back(a->children[i]);
	}
	for (int i = 0; i < case_label.size(); ++i) {
		if (cases[i]->type == AST_CASE) {
			auto value = visit_bin_op_node(cases[i]->children[0]);
			string val__ = "<DEFAULT>";
			if (value) {
				val__ = value->name;
			}
			opcs.push_back(OperatorCommand(make_label(), {OPER_IF, name, "=", val__, OPER_GOTO, case_label[i]}));
		} else if (cases[i]->type == AST_DEFAULT) {
			opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, end_l}));
		}
	}
	visit_case_node(cases, case_label, end_l);
	RENUL;
}

PName Compiler::visit_pos_value(AST* a) {
	normal_debug();
	string lis_name = visit_member_node(a->children[0])->name;
	string value    = visit_bin_op_node(a->children[1])->name;
	string vn       = make_var_name();
	opcs.push_back(OperatorCommand(make_label(), {vn, "=", lis_name, "+", value}));
	opcs.push_back({make_label(), {OPER_LOAD, vn, lis_name, value}});
	return new Name(vn);
}

PName Compiler::visit_list_node(AST* a) {
	normal_debug();
	string vn = make_var_name();
	vector<string> values;
	for (auto i : a->children)
		values.push_back(visit_bin_op_node(i)->name);
	opcs.push_back(OperatorCommand(make_label(), {OPER_LIST, vn, to_string(values.size())}));
	for (int i = 0; i < values.size(); ++i)
		opcs.push_back(OperatorCommand(make_label(), {OPER_ASSI, vn, to_string(i), values[i]}));
	return new Name(vn);
}

PName Compiler::visit_member_node(AST* a) {
    if (match_type(a, AST_GET_VALUE))
	    return visit_pos_value(a);
    if (match_type(a, AST_ID))
	    return visit_id_node(a);
    if (match_type(a, AST_MEM_MALLOC))
	    return visit_mem_malloc_node(a);
	if (match_type(a, AST_FUNC_CALL)) {
		auto base_name = a->children[0]->children[0];
		auto this_addr = visit_member_node(base_name)->name;
		opcs.push_back(OperatorCommand(make_label(), {OPER_PARAM, this_addr}));
		for (int i = 1; i < a->children.size(); ++i)
			opcs.push_back(OperatorCommand(make_label(), {OPER_PARAM, visit_bin_op_node(a->children[i])->name}));
		opcs.push_back(OperatorCommand(make_label(), {OPER_CALL, a->children[0]->children[1]->data.data}));
		string tvn = make_var_name();
		opcs.push_back(OperatorCommand(make_label(), {OPER_GET_RET_VAL, tvn}));
		return new Name(tvn);
	}
  if (match_type(a, AST_MEMBER)) {
	 	auto base = a->children[0];
		auto memb = a->children[1];
		auto left = visit_member_node(base)->name;
		string t = a->parent_name;
		string d = memb->data.data;
		auto offset = to_string(objects[t][d]);
		auto tmpv = make_var_name();
		opcs.push_back(OperatorCommand(make_label(), {tmpv, "=", left, "+", offset}));
		return new Name(tmpv);
	}
  err_out(COMPILE_TIME_ERROR, "unknow type %s", a->type.c_str());
	RENUL;
}

string Compiler::make_label() { normal_debug(); return "l" + to_string(++label_cnt); }

string Compiler::make_var_name() {
	normal_debug();
	return "v" + to_string(++var_cnt);
}

PName Compiler::visit_for_loop_node(AST* a) {
	normal_debug();
	string lb = make_loop_begin_label();
	string le = make_loop_end_label();
	string init = visit_var_def_node(a->children[0])->name;
	string end  = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(lb, {OPER_IF, OPER_NOT, end, OPER_GOTO, le}));
	visit_atom(a->children[3], lb, le);
	visit_bin_op_node(a->children[2]);
	opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, lb}));
	opcs.push_back(OperatorCommand(le, {OPER_NOP}));
	RENUL;
}

PName Compiler::visit_while_loop_node(AST* a) {
	normal_debug();
	string lb = make_loop_begin_label();
	string le = make_loop_end_label();
	string cond = visit_bin_op_node(a->children[0])->name;
	opcs.push_back(OperatorCommand(lb, {OPER_IF, OPER_NOT, cond, OPER_GOTO, le}));
	visit_atom(a->children[1], lb, le);
	opcs.push_back(OperatorCommand(make_label(), {OPER_GOTO, lb}));
	opcs.push_back(OperatorCommand(le, {OPER_NOP}));
	RENUL;
}

PName Compiler::visit_bin_op_node(AST* a) {
	normal_debug();
	if (!a) {
		err_out(COMPILE_TIME_ERROR, "meet null node", 0);
	} if (match_type(a, AST_DIGIT) || match_type(a, AST_STRING)) {
		string vn = make_var_name();
		opcs.push_back(OperatorCommand(make_label(), {vn, "=", a->data.data}));
		return new Name(vn);
	} else if (match_type(a, AST_ID)) {
		return visit_id_node(a);
	} else if (match_type(a, AST_INC)) {
		return visit_self_add_node(a);
	} else if (match_type(a, AST_DEC)) {
		return visit_self_sub_node(a);
	} else if (match_type(a, AST_FUNC_CALL)) {
		return visit_call_node(a);
	} else if (match_type(a, AST_SELF_ADD)) {
		return visit_self_add_node(a);
	} else if (match_type(a, AST_SELF_SUB)) {
		return visit_self_sub_node(a);
	} else if (match_type(a, AST_SELF_DIV)) {
		return visit_self_div_node(a);
	} else if (match_type(a, AST_SELF_MUL)) {
		return visit_self_mul_node(a);
	} else if (match_type(a, AST_LIST)) {
		return visit_list_node(a);
	} else if (match_type(a, AST_GET_VALUE)) {
		return visit_pos_value(a);
	} else if (match_type(a, AST_MEMBER) || match_type(a, AST_FUNC_CALL)) {
		return visit_member_node(a);
	} else if (match_type(a, AST_ID)) {
		return visit_id_node(a);
	} else if (match_type(a, AST_MEM_MALLOC))  {
		return visit_mem_malloc_node(a);
	} else if (match_type(a, AST_CONV)) {
		return visit_type_cast_node(a);
	}
	cout << a->type << endl;
	string left = visit_bin_op_node(a->children[0])->name;
	string right = visit_bin_op_node(a->children[1])->name;
	string vn = make_var_name();
	opcs.push_back(OperatorCommand(make_label(), {vn, "=", left, a->data.data, right}));
	return new Name(vn);
}

PName Compiler::visit_self_add_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	string id = visit_member_node(a->children[0])->name;
	opcs.push_back(OperatorCommand(make_label(), {id, "=", id, "+", "1"}));
	return new Name(id);
}

PName Compiler::visit_self_sub_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	string id = visit_member_node(a->children[0])->name;
	opcs.push_back(OperatorCommand(make_label(), {id, "=", id, "-", "1"}));
	return new Name(id);
}

PName Compiler::visit_self_div_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	if (a->children.size() != 2) {
		cout << "Compile time error: not a good struct\n";
		exit(-1);
	}
	auto name = visit_member_node(a->children[0])->name;
	auto data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_DIV, name, data}));
	return new Name(name);
}

PName Compiler::visit_self_mul_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	if (a->children.size() != 2) {
		cout << "Compile time error: not a good struct\n";
		exit(-1);
	}
	auto name = visit_member_node(a->children[0])->name;
	auto data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_MUL, name, data}));
	return new Name(name);
}

PName Compiler::visit_self_mod_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	if (a->children.size() != 2) {
		cout << "Compile time error: not a good struct\n";
		exit(-1);
	}
	auto name = visit_member_node(a->children[0])->name;
	auto data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_MOD, name, data}));
	return new Name(name);
}

PName Compiler::visit_mem_malloc_node(AST* a) {
	normal_debug();
	if (!a) RENUL;
	string name = make_this_name(a->data.data);
	string ret_name = visit_call_node(a->children[0])->name;
	opcs.push_back(OperatorCommand(make_label(), {name, "=", ret_name}));
	return new Name(name);
}

PName Compiler::visit_ret_node(AST* a) {
	normal_debug();
	if (!a) RENUL;
	auto i = a->children[0];
	if (i) {
		string id = visit_bin_op_node(i)->name;
		opcs.push_back(OperatorCommand(make_label(), {OPER_RET, id}));
	} else {
		opcs.push_back(OperatorCommand(make_label(), {OPER_LEAVE}));
	}	
	RENUL;
}

PName Compiler::visit_call_node(AST* a) {
	auto function_address = a->children[0];
	if (function_address->type == AST_MEMBER) {
		return visit_member_node(a);
	}
	normal_debug();
	if (a == nullptr) {
		cout << "Compile time error: meet null node" << endl;
		exit(-1);
	}
	vector<string> value_list;
	string func_name = visit_member_node(a->children[0])->name;
	for (int i = 1; i < a->children.size(); ++i) {
		auto t = a->children[i];
		if (!t) continue;
		auto n = visit_bin_op_node(t);
		if (!n) {
			err_out(COMPILE_TIME_ERROR, "meet null node", 0);
		}
		value_list.push_back(n->name);
	}
	vector<string> pr_val;
	if (value_list.size() > 0) {
		pr_val.push_back(OPER_PARAM);
		for (auto i : value_list) pr_val.push_back(i);
		opcs.push_back(OperatorCommand(make_label(), pr_val));
	}
	opcs.push_back(OperatorCommand(make_label(), {OPER_CALL, func_name}));
	string ret_val_sc = make_var_name();
	opcs.push_back(OperatorCommand(make_label(), {OPER_LD_RET, ret_val_sc}));
	return new Name(ret_val_sc);
}

PName Compiler::visit_var_def_node(AST* a) {
	normal_debug();
	if (!a) {
		printf("Error\n");
		exit(-1);
	}
	string vn = a->data.data;
	auto val_type = a->children[0];
	auto vd = a->children[1];
	opcs.push_back(OperatorCommand(make_label(), {OPER_VAR, vn}));
	if (vd) {
		string tn_ = visit_bin_op_node(vd)->name;
		opcs.push_back(OperatorCommand(make_label(), {vn, "=", tn_}));
	}
	if (val_type) var_type_map[vn] = val_type->data.data;
	return new Name(vn);
}

PName Compiler::visit_var_assign_node(AST* a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	string vn = visit_member_node(a->children[0])->name;
	string data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_ASSI, vn, data}));
	return new Name(vn);
}

PName Compiler::visit_atom(AST* a, string l_begin, string l_end) {
	normal_debug();
	if (!a) RENUL;
	string type = a->type;
	if (type == AST_FUNCTION_DEFINE) return visit_function_define_node(a);
	else if (type == AST_CLASS) return visit_class_node(a);
	else if (type == AST_FOR_LOOP) return visit_for_loop_node(a);
	else if (type == AST_WHILE_LOOP) return visit_while_loop_node(a);
	else if (type == AST_FUNC_CALL) return visit_call_node(a);
	else if (type == AST_MEM_MALLOC) return visit_mem_malloc_node(a);
	else if (type == AST_RETURN) return visit_ret_node(a);
	else if (type == AST_SWITCH) return visit_switch_node(a);
	else if (type == AST_GET_VALUE) return visit_pos_value(a);
	else if (match_type(a, AST_CONV)) return visit_type_cast_node(a);
	else if (type == AST_STRING) return visit_string_node(a);
	else if (type == AST_LIST) return visit_list_node(a);
	else if (type == AST_DIGIT) return visit_digit_node(a);
	else if (type == AST_BIN_OP) return visit_bin_op_node(a);
	else if (type == AST_SELF_ADD) return visit_added_by(a);
	else if (type == AST_SELF_DIV) return visit_self_div_node(a);
	else if (type == AST_SELF_MUL) return visit_self_mul_node(a);
	else if (type == AST_SELF_SUB) return visit_subbed_by(a);
	else if (type == AST_INC) return visit_self_add_node(a);
	else if (type == AST_DEC) return visit_self_sub_node(a);
	else if (type == AST_NOT) return visit_make_not_node(a);
	else if (type == AST_VAR_DEF) return visit_var_def_node(a);
	else if (type == AST_ASSIGN) return visit_var_assign_node(a);
	else if (type == AST_IF) return visit_if_node(a, l_begin, l_end);
	else if (type == AST_CONTINUE) return visit_continue_node(l_begin, l_end);
	else if (type == AST_BREAK) return visit_break_node(l_begin, l_end);
	else if (type == AST_BLOCK) {
		for (auto i : a->children)
			visit_atom(i, l_begin, l_end);
	} else {
		err_out(COMPILE_TIME_ERROR, "unknown tree %s", type.c_str());
	}
	RENUL;
}

void Compiler::consume(AST* a, string type, string func) {
	normal_debug();
	if (!a) { cout << "Error" << endl; exit(-1); }
	if (!a || a->type != type) {
		cout << "Compile time error: want' " << type << "' meet " << a->type << endl;
		exit(-1);
	}
}

Name *Compiler::visit_type_cast_node(AST *a) {
	normal_debug();
	string vn = make_var_name();
	string type = a->children[0]->data.data;
	string val  = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {vn, "=", OPER_TYPE_CAST, val, type}));
	return new Name(vn);
}

Name* Compiler::visit_class_node(AST *a) {
	if (!a || a->type != AST_CLASS) {
		err_out(COMPILE_TIME_ERROR, "not a class", 0);
	}
 	normal_debug();
	string class_name = a->data.data;
	if (class_name.empty()) {
		err_out(COMPILE_TIME_ERROR, "no class name", 0);
	}
	class_size_map[class_name] = a->class_size;
	objects[class_name] = ObjectInfo(class_name);
	auto parent = a->children;
	vector<AST*> member;
	vector<AST*> func;
	AST* constructor = a->id_node;
	if (!constructor) {
		err_out(COMPILE_TIME_ERROR, "no init function", 0);
	}
	map<string, AST*> init_value;
	opcs.push_back(OperatorCommand(make_label(), {CLASS_BEG, class_name}));
	int offset_base = 1;
	for (auto i : parent) {
		if (i->type == AST_VAR_DEF) {
			auto tmp_name = i->data.data;
			objects[class_name].add_offset(tmp_name, offset_base++);
			member.push_back(i);
		}
		else if (i->type == AST_FUNCTION_DEFINE) {
			func.push_back(i);
		}
	}
	for (auto i : member) {
		auto name = i->data.data;
		auto val = i->children[1];
		auto typ = make_id_type_node(i->children[0]).rootType;
		if (val) init_value[name] = val;
		opcs.push_back(OperatorCommand(make_label(), {OPER_SPACE,name,to_string(class_size_map[typ])}));
	}
	opcs.push_back(OperatorCommand(make_label(), {CLASS_END}));
	
	// build constructor function
	auto constructor_val_list = constructor->children[0]->children;
	auto constructor_bdy_list = constructor->children[1]->children;
	auto ret_type = make_id_type_node(constructor->children[2]);
	if (ret_type.rootType != "void" && ret_type.rootType != "int") {
		// cout << ret_type.rootType << endl;
		err_out(COMPILE_TIME_ERROR, "\"constructor\" is not a legitimate constructor", 0);
	}
	opcs.push_back(OperatorCommand(make_label(), {FUNC_BEG, class_name + "$" + "constructor"}));
	opcs.push_back(OperatorCommand(make_label(), {OPER_SPACE, to_string(a->class_size)}));
	for (int i = 0; i < constructor_val_list.size(); ++i) {
		auto cur_val = constructor_val_list[i]->data.data;
		opcs.push_back(OperatorCommand(make_label(), {OPER_LOAD, cur_val, "#" + to_string(i)}));
	}
	for (auto i = init_value.begin(); i != init_value.end(); ++i) {
		auto name = i->first;
		auto data = visit_bin_op_node(i->second)->name;
		auto offset = to_string(objects[class_name][name]);
		auto tmp = make_var_name();
		opcs.push_back(OperatorCommand(make_label(), {tmp, "=", "this", "+", offset}));
		opcs.push_back(OperatorCommand(make_label(), {OPER_LOAD, tmp, data}));
	}
	for (auto i : constructor_bdy_list) {
		visit_atom(i, "<ERROR>", "<ERROR>");
	}
	opcs.push_back(OperatorCommand(make_label(), {OPER_RET, "this"}));
	opcs.push_back(OperatorCommand(make_label(), {FUNC_END}));
	
	// build member function
	for (auto i : func) {
		string name = class_name + "$" + i->data.data;
		cout << "build class function member: " << name << endl;
		auto value_list = i->children[0];
		auto body = i->children[1]->children;
		opcs.push_back(OperatorCommand(make_label(), {FUNC_BEG, name}));
		for (int j = 0; j < value_list->children.size(); ++j) {
			auto van = value_list->children[j]->data.data;
			opcs.push_back(OperatorCommand(make_label(), {OPER_LOAD, van, "#" + to_string(j)}));
		}
		for (auto j : body) {
			visit_atom(j, "<ERROR>", "<ERROR>");
		}
		opcs.push_back(OperatorCommand(make_label(), {FUNC_END}));
	}
	RENUL;
}


Name *Compiler::visit_added_by(AST *a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	if (a->children.size() != 2) {
		cout << "Compile time error: not a good struct\n";
		exit(-1);
	}
	auto name = visit_member_node(a->children[0])->name;
	auto data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_ADD, name, data}));
	return new Name(name);
}

Name *Compiler::visit_subbed_by(AST *a) {
	normal_debug();
	if (!a) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	if (a->children.size() != 2) {
		cout << "Compile time error: not a good struct\n";
		exit(-1);
	}
	auto name = visit_member_node(a->children[0])->name;
	auto data = visit_bin_op_node(a->children[1])->name;
	opcs.push_back(OperatorCommand(make_label(), {OPER_SUB, name, data}));
	return new Name(name);
}

Name *Compiler::visit_make_not_node(AST *a) {
	normal_debug();
	if (a == nullptr) {
		cout << "Compile time error: meet null node\n";
		exit(-1);
	}
	auto name = a->children.size() > 0? a->children[0] : nullptr;
	if (name == nullptr) {
		cout << "Compile time error: node not supposed\n";
		exit(-1);
	}
	auto id = visit_member_node(name);
	string _str_name = id? id->name : "UNKNOWN_SYMBOL" ;
	opcs.push_back(OperatorCommand(make_label(), {OPER_NOT, _str_name}));
	return new Name(_str_name);
}

class ThreeCodeObject {
public:
	string name;
	int id;
	ThreeCodeObject();
	ThreeCodeObject(vector<OperatorCommand>);
	vector<OperatorCommand> opcs;
};

ThreeCodeObject::ThreeCodeObject() {}

ThreeCodeObject::ThreeCodeObject(vector<OperatorCommand> opcs) {
	this->opcs = opcs;
}

class ThreeCodeFunction {
public:
	string name;
	int id;
	ThreeCodeFunction();
	ThreeCodeFunction(vector<OperatorCommand>);
	vector<OperatorCommand> opcs;
};

ThreeCodeFunction::ThreeCodeFunction(vector<OperatorCommand> opcs) {
	this->opcs = opcs;
}

ThreeCodeFunction::ThreeCodeFunction() {}

class MainCompiler {
public:
	MainCompiler(vector<OperatorCommand>);
	vector<OperatorCommand> opcs;
	OperatorCommand *current;
	unordered_map<int, ThreeCodeObject> object_rec;
	unordered_map<int, ThreeCodeFunction> func_rec;
	unordered_map<string, int> label_map;
	void load();
};

void MainCompiler::load() {
	int i = 0, obj_cnt = 0, func_cnt = 0;
	while (i < opcs.size()) {
		if (opcs[i].codes[0] == CLASS_BEG) {
			string name = opcs[i].codes[1];
			++i;
			vector<OperatorCommand> temp;
			while (opcs[i].codes[0] != CLASS_END)
				temp.push_back(opcs[i++]);
			++i;
			object_rec[obj_cnt] = ThreeCodeObject(temp);
			object_rec[obj_cnt].id = obj_cnt;
			object_rec[obj_cnt].name = name;
			++obj_cnt;
		}
		if (opcs[i].codes[0] == FUNC_BEG) {
			string name = opcs[i].codes[1];
			++i;
			vector<OperatorCommand> temp;
			while (opcs[i].codes[0] != CLASS_END)
				temp.push_back(opcs[i++]);
			++i;
			func_rec[obj_cnt] = ThreeCodeFunction(temp);
			func_rec[obj_cnt].id = func_cnt;
			func_rec[obj_cnt].name = name;
			++func_cnt;
		}
	}
}

MainCompiler::MainCompiler(vector<OperatorCommand> opcs) {
	this->opcs = opcs;
}

#endif