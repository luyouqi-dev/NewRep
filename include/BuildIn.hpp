//
// Created by User on 2025/8/21.
//

#ifndef PROGRAMLANGUAGEINCPP_BUILDIN_HPP
#define PROGRAMLANGUAGEINCPP_BUILDIN_HPP
#include <string>
#include <vector>
#include <unordered_map>
using std::string;
using std::vector;
using std::unordered_map;

enum Assembly {
	_add = 0x0a,
	_sub = 0x0b,
	_mul = 0x0c,
	_div = 0x0d,
	_mod = 0x0f,
	_pow = 0x0e,
	_xor = 0xef,
	_left_mv = 0x98f3e,
	_right_mv = 0x911fe,
	
	_dup = 0x0909090,
	_psh = 0xee, // 1
	_pop = 0xefef,
	_a_psh = 0xae, // 1
	_a_pop = 0xaf,
	_imm = 0xaaff, // 1
	_i_load = 0x123, // 1
	_i_stor = 0x1123, // 2
	_param = 0xfe567,
	_sddl = 0x98f123,
	_hsst = 0x98fffe,
	_new = 0xeabcd,
	_sdup = 0x9ffffe3,
	
	_or = 0x13ef,
	_and = 0x345ef,
	_not = 0xeeeeff,
	_cbg = 0xcccc, // comp bigger
	_cls = 0xcef,  // comp less
	_ceq = 0xefef1,// comp eq
	_cne = 0xeef44,// comp not eq
	
	_jt = 0x1ffee, // 1
	_jmp = 0x4f4fe3, // 1
	_system_call = 0x7c00,
	
	_call = 0x88feabc, // 1
	_scall = 0x98af,
	_ret   = 0x8899fe,
	_lea = 0x8844fed,
	
};

enum SysCall {
	CallOpenFile = 0x9c00,
	
};

enum Opera {
	SET_MAIN, SET_CLASS, SET_FUNC
};

enum FunctionType {
	FT_USER_DEFINE, FT_LIB, FT_BUILD_IN
};

vector<string> build_in_function = {
	"print",
	"input",
	"to_string",
	"to_int",
	"append", /*list.append*/
	"size"/*list.size*/,
	"pop" /*stack.pop*/,
	"push" /*stack.push*/,
	"top" /*stack.top*/,
};

vector<string> build_in_class = { "int", "double", "bool", "string", "list", "stack", "void", "char" };

enum BuildInFunction {
	PRINT_ = 0XEF4,
	INPUT_ = 0X1AC,
	TO_STRING_ = 0XACE,
	TO_INTEGER_ = 0XACEF4,
	LSIZE  = 0X0EFC,
	LAPPEND = 0X11FFEA,
	SPOP  = 0X11001,
	SPSH  = 0X001100,
	STOP = 0X121212
};

enum BuildInClass {
	BS_INT, BS_STR, BS_CHAR, BS_DOUBLE, BS_BOOL, BS_LIST, BS_STACK
};

unordered_map<string, vector<int>> vm_build_in_fn_map = {
	/*  {name,         {id,      parent_class, value_size, ret_type}}  */
	{"print",     {PRINT_,      -1,        1,         _lea}},
	{"input",     {INPUT_,      -1,        1,         _ret}},
	{"to_string", {TO_STRING_,  -1,        1,         _ret}},
	{"to_int",    {TO_INTEGER_, -1,        1,         _ret}},
	{"size",      {LSIZE,       BS_LIST,   1,         _ret}},
	{"pop",       {SPOP,        BS_STACK,  1,         _ret}},
	{"push",      {SPSH,        BS_STACK,  2,         _lea}},
	{"top",       {STOP,        BS_STACK,  1,         _ret}},
	{"append",    {LAPPEND,     BS_LIST,   2,         _lea}}
};

unordered_map<string, vector<int>> vm_build_in_class = {
	//  name           id       size (-1 = autoSize)
	{"int",    {BS_INT   ,  1}},
	{"string", {BS_STR   ,  1}},
	{"char",   {BS_CHAR  ,  1}},
	{"double", {BS_DOUBLE,  1}},
	{"bool",   {BS_BOOL  ,  1}},
	{"list",   {BS_LIST  , -1}},
	{"stack",  {BS_STACK , -1}}
};

#endif //PROGRAMLANGUAGEINCPP_BUILDIN_HPP
