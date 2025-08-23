#ifndef COLOR
#define COLOR
#include <iostream>
using namespace std;
const string
PARSE_TIME_ERROR = "ParseTimeError:",
COMPILE_TIME_ERROR    = "CompileTimeError:",
RUNTIME_ERROR    = "RuntimeError:";
#define ERROR Color256output("[ERROR]:", 49, 196, 0); printf(" ");
#define DEBUG Color256output("[DEBUG]:", 30, 120, 0); printf(" ");
#define WARN  Color256output("[WARN]:", 43, 172, 0); printf(" ");
bool is_debug = false;
bool err_is_exit = true;
bool war_is_exit = false;
#define debug_out(fmt, ...) \
	{if (is_debug) {        \
		DEBUG;                \
		printf("FILE(%s) FUNCTION(%s) LINE(%d) DATA = (", __FILE__, __FUNCTION__, __LINE__); \
		printf(fmt, __VA_ARGS__);              \
        printf(")\n");                                             \
	}}

#define normal_debug() debug_out("", "")

#define warn_out(fmt, ...) \
	{WARN;                 \
	printf("FILE(%s) FUNCTION(%s) LINE(%d) DATA = ", __FILE__, __FUNCTION__, __LINE__); \
	printf(fmt, __VA_ARGS__)       ;                                                     \
	if (war_is_exit) exit(-1);}

#define err_out(et, fmt, ...) \
	{ERROR;               \
	printf("FILE(%s) FUNCTION(%s) LINE(%d) DATA = ", __FILE__, __FUNCTION__, __LINE__); \
    cout << et << " ";                      \
	printf(fmt, __VA_ARGS__)   ;                                                         \
	if (err_is_exit) exit(-1);}

using namespace std;

struct Color {
	int bc, fc;
	Color(int a, int b) {
		bc = a;
		fc = b;
	}
	Color() {  }
};

void Color256output(string d, int fc, int bc, char end = '\n') {
	cout << "\033[" << "38;5;" << fc << "m";
	cout << "\033[" << "48;5;" << bc << "m";
	cout << d << "\033[m";
	if (end != 0) cout << end;
}


void Color256output(string d, Color col, char end = '\n') {
	cout << "\033[" << "38;5;" << col.fc << "m";
	cout << "\033[" << "48;5;" << col.bc << "m";
	cout << d << "\033[m";
	if (end != 0) cout << end;
}

#endif 