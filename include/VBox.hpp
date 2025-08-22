#ifndef VBOX
#define VBOX
#include <iostream>
#include <vector>
#include <stack>
#include <map>
#include <algorithm>
#include <cstring>
#include "BuildIn.hpp"
#include <cmath>
#define ZERO_I(x) memset(x, 0, 4 * sizeof(x))
#define SS (1024*1024)
#define INPUT_ID 0X4F567
#define PRINT_ID 0X888FE
#define HEAP_SIZE (1024*1024*1024)
#define CUR_FRAME frame.top()
#define PS (1024*1024*1024)
using std::stack;
using std::vector;
using std::cout;
using std::map;
using std::string;


struct Object {
	int id, size;
	vector<int> parent_id;
	map<int, int> member_map;
};

struct Function {
	vector<int> codes;
	int id, parent_class;
	int value_list_size;
	FunctionType type = FT_USER_DEFINE;
};

struct Frame {
	Function* func_point;
	vector<int> body;
	int *st, *sp, *sb, pc, a, *smid;
	Frame();
	Frame(Function*);
	void psh(int);
	int pop();
	void sstor(int, int);
	int get();
	int get(int);
	int top();
};

int Frame::top() { return *sp; }

int Frame::get() { if (pc > body.size()) return _lea; return body[pc++]; }

Frame::Frame() {
	func_point = nullptr;
	st = new int[SS];
	sb = st;
	smid = sb + SS / 2;
	sp = smid;
	pc = 0;
}

Frame::Frame(Function *point) {
	func_point = point;
	st = new int[SS];
	sb = st;
	smid = sb + SS / 2;
	sp = smid;
	pc = 0;
	body = point->codes;
}

void Frame::psh(int _a) {
	*sp = _a;
	++sp;
}

int Frame::pop() {
	int v = *sp;
	sp--;
	return v;
}

void Frame::sstor(int offset, int value) {
	*(smid - offset) = value;
}

int Frame::get(int offset) {
	return *(smid - offset);
}

class VirtualMachine {
public:
	VirtualMachine(vector<int>);
private:
	vector<int> opcs;
	vector<Function*> functions;
	vector<Object*> objects;
	stack<Frame> frame;
	vector<int> func_value;
	map<int, int> size_rec;
	int *heap, *htop, main_point = -1;
	int heap_stor(int);
	void setup_build_in();
	void load_object();
	void init();
	int execute();
	void crate(int);
	void leave();
	int store_string(string);
	void call_build_in(int);
	void append_value(int, int);
	string get_string(int);
	Function* find_function(int);
	Object* find_class(int);
};

void VirtualMachine::append_value(int object_address, int value) {
	int& size_ref = *(heap + object_address + LIST_SIZE_OFFSET);
  *(heap + object_address + (++size_ref)) = value;
}

void VirtualMachine::call_build_in(int bfid) {
	switch (bfid) {
		case PRINT_ : {
			int addr = CUR_FRAME.pop();
			string data = get_string(addr);
			puts(data.c_str());
			break;
		}
		case INPUT_ : {
			string prompt, input_value;
			prompt = get_string(CUR_FRAME.pop());
			puts(prompt.c_str());
			getline(std::cin, input_value);
			CUR_FRAME.psh(store_string(input_value));
			break;
		}
		case TO_STRING_ : {
			int num = CUR_FRAME.pop();
			CUR_FRAME.psh(store_string(std::to_string(num)));
			break;
		}
		case TO_INTEGER_ : {
			int addr = CUR_FRAME.pop();
			string s = get_string(addr);
			CUR_FRAME.psh(std::stoi(s));
			break;
		}
		case LSIZE : {
			int addr = CUR_FRAME.pop();
			int si   = *(heap + addr + LIST_SIZE_OFFSET);
			CUR_FRAME.psh(si);
			break;
		}
		case LAPPEND : {
			int object_address = CUR_FRAME.pop();
			int value = CUR_FRAME.pop();
			append_value(object_address, value);
			break;
		}
		case SPOP : {
			int object_address = CUR_FRAME.pop();
			(*(heap + object_address + STACK_TOP_OFFSET))--;
			break;
		}
		case SPSH : {
			int object_address = CUR_FRAME.pop();
			int value = CUR_FRAME.pop();
			append_value(object_address, value);
			break;
		}
		case STOP  : {
			int object_address = CUR_FRAME.pop();
			int size = *(heap + object_address + 1);
			if (size > 0) {
				int top_value = *(heap + object_address + size);
				CUR_FRAME.psh(top_value);
			} else {
				std::cerr << "STOP error: empty stack" << std::endl;
				CUR_FRAME.psh(0);
			}
			break;
		}

		default: {
			cout << "Error: build-in function not found\n";
			exit(-1);
		}
	}
}

void VirtualMachine::setup_build_in() {
	for (auto i : vm_build_in_fn_map) {
		auto bi_fn = new Function;
		bi_fn->id = i.second[0];
		bi_fn->parent_class = i.second[1];
		bi_fn->value_list_size = i.second[2];
		bi_fn->codes.push_back(i.second[3]);
		functions.push_back(bi_fn);
	}
	for (auto i : vm_build_in_class) {
		auto bcl = new Object;
		bcl->id   = i.second[0];
		bcl->size = i.second[1];
		objects.push_back(bcl);
	}
}

string VirtualMachine::get_string(int addr) {
	string r;
	int i = 0;
	while (*(heap + addr + i))
		r += *(heap + addr + i++);
	return r;
}

int VirtualMachine::store_string(std::string s) {
	s += '\0';
	int req_size = s.size();
	if (htop + req_size >= heap + HEAP_SIZE) {
		std::cerr << "Heap overflow in string storage" << std::endl;
		exit(EXIT_FAILURE);
	}
	int start_offset = htop - heap;
	for (int j = 0; j < req_size; ++j) {
		*htop++ = s[j];
	}
	return start_offset;
}


Object* VirtualMachine::find_class(int id) {
	for (auto i : objects)
		if (i->id == id)
			return i;
	return nullptr;
}

int VirtualMachine::heap_stor(int object_id) {
	int top = htop - heap;
	int* tmp_top = htop;
	auto obj = find_class(object_id);
	if (!obj) {
		cout << "object " << object_id << " not found\n";
		exit(-1);
	}
	int si = obj->size;
	if (si == -1) si = CUR_FRAME.pop();
	htop += si;
	for (auto i = obj->member_map.begin(); i != obj->member_map.end(); ++i)
		*(tmp_top + i->first) = heap_stor(i->second);
	size_rec[top] = si;
	return top;
}

VirtualMachine::VirtualMachine(vector<int> opcs) {
	this->opcs = opcs;
	init();
}

int VirtualMachine::execute() {
	while (!frame.empty()) {
		int i = CUR_FRAME.get();
		switch (i) {
			case _add : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a + b);
				break;
			}
			case _sub : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a - b);
				break;
			}
			case _mul : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a * b);
				break;
			}
			case _sdup: {
				CUR_FRAME.psh(CUR_FRAME.get(CUR_FRAME.get()));
				break;
			}
			case _div : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a / b);
				break;
			}
			case _new: {
				int class_id = CUR_FRAME.pop();
				CUR_FRAME.psh(heap_stor(class_id));
				break;
			}
			case _scall: {
				int id = CUR_FRAME.pop();
				crate(id);
				for (auto h : func_value)
					CUR_FRAME.psh(h);
				break;
			}
			case _dup: {
				CUR_FRAME.psh(CUR_FRAME.top());
				break;
			}
			case _mod : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a % b);
				break;
			}
			case _pow : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(pow(a,b));
				break;
			}
			case _xor : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a ^ b);
				break;
			}
			case _left_mv : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a << b);
				break;
			}
			case _right_mv : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a >> b);
				break;
			}
			case _psh : {
				CUR_FRAME.psh(CUR_FRAME.get());
				break;
			}
			case _pop : {
				CUR_FRAME.pop();
				break;
			}
			case _a_psh : {
				CUR_FRAME.psh(CUR_FRAME.a);
				break;
			}
			case _a_pop : {
				CUR_FRAME.a = CUR_FRAME.pop();
				break;
			}
			case _imm : {
				CUR_FRAME.a = heap[CUR_FRAME.pop()];
				break;
			}
			case _i_load : {
				CUR_FRAME.psh(CUR_FRAME.get(CUR_FRAME.pop()));
				break;
			}
			case _i_stor : {
				CUR_FRAME.sstor(CUR_FRAME.pop(), CUR_FRAME.pop());
				break;
			}
			case _param : {
				int size = CUR_FRAME.pop();
				func_value.clear();
				while (size--) func_value.push_back(CUR_FRAME.pop());
				std::reverse(func_value.begin(), func_value.end());
				break;
			}
			case _hsst : {
				string s;
				char c = CUR_FRAME.pop();
				do {
					s += c;
					c = CUR_FRAME.pop();
				} while (c);
				CUR_FRAME.psh(store_string(s));
				break;
			}
			case _or : {
				CUR_FRAME.psh(CUR_FRAME.pop() | CUR_FRAME.pop());
				break;
			}
			case _and : {
				CUR_FRAME.psh(CUR_FRAME.pop() & CUR_FRAME.pop());
				break;
			}
			case _not : {
				CUR_FRAME.psh(~CUR_FRAME.pop());
				break;
			}
			case _cbg : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a > b);
				break;
			}
			case _cls : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a < b);
				break;
			}
			case _ceq : {
				CUR_FRAME.psh(CUR_FRAME.pop() == CUR_FRAME.pop());
				break;
			}
			case _cne : {
				CUR_FRAME.psh(CUR_FRAME.pop() != CUR_FRAME.pop());
				break;
			}
			case _cebg : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a >= b);
				break;
			}
			case _cels : {
				int b = CUR_FRAME.pop();
				int a = CUR_FRAME.pop();
				CUR_FRAME.psh(a <= b);
				break;
			}
			case _jt : {
				if (CUR_FRAME.pop()) CUR_FRAME.pc = CUR_FRAME.get();
				break;
			}
			case _jmp : {
				CUR_FRAME.pc = CUR_FRAME.get();
				break;
			}
			case _call : {
				int id = CUR_FRAME.get();
				auto fn = find_function(id);
				crate(id);
				for (auto h : func_value)
					CUR_FRAME.psh(h);
				if (fn->type == FT_BUILD_IN)
					call_build_in(id);
				break;
			}
			case _ret : {
				int rev = CUR_FRAME.get();
				leave();
				CUR_FRAME.psh(rev);
				break;
			}
			case _lea : {
				leave();
				break;
			}
			default: {
				printf("Unknown operator command %d \n", i);
				exit(-1);
				break;
			}
		}
	}
	return 0;
}

void VirtualMachine::crate(int id) {
	frame.emplace(find_function(id));
}

void VirtualMachine::leave() {
	frame.pop();
}

Function *VirtualMachine::find_function(int id) {
	for (auto i : functions)
		if (i->id == id)
			return i;
	return nullptr;
}

void VirtualMachine::init() {
	heap = new int[HEAP_SIZE];
	htop = heap;
	load_object();
	if (!main_point) cout << "NoMain\n", exit(-1);
	setup_build_in();
	crate(main_point);
}

void VirtualMachine::load_object() {
	int i = 0;
	while (i < opcs.size()) {
		int op = opcs[i++];
		if (op == SET_MAIN)
			main_point = opcs[i++];
		else if (op == SET_FUNC) {
			int id, parent = -1, val_size, body_size;
			vector<int> body;
			id = opcs[i++];
			parent = opcs[i++];
			val_size = opcs[i++];
			body_size = opcs[i++];
			for (int j = 0; j < body_size; ++j)
				body.push_back(opcs[i++]);
			auto nfn = new Function;
			nfn->codes = body;
			nfn->id = id;
			nfn->value_list_size = val_size;
			nfn->parent_class = parent;
			functions.push_back(nfn);
		} else if (op == SET_CLASS) {
			int id, size, psize;
			vector<int> par, fun;
			map<int, int> memb;
			id = opcs[i++];
			size = opcs[i++];
			psize = opcs[i++];
			for (int j = 0; j < psize; ++j)
				par.push_back(opcs[i++]);
			for (int j = 0; j < size; ++j)
				memb[opcs[i++]] = opcs[i++];
			auto nob = new Object;
			nob->id = id;
			nob->size = size;
			nob->member_map = memb;
			nob->parent_id = par;
			objects.push_back(nob);
		}
	}
}

#endif