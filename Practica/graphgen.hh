#include <string>
#include <set>
#include <sstream>

using namespace std;

string intToStr(int i);
int strToInt(string s);

struct function{
	string module;
	string name;
	int params;
	
	bool operator<(const function &other) const{
    	return (module + name + intToStr(params)) < (other.module + other.name + intToStr(other.params));
    }	
};

struct functionCall{
	function* from;
	function* to;
	
	bool operator<(const functionCall &other) const{
	 	return (from < other.from)||(to < other.to);
    }
};

struct process{
	string pFrom;
	string pTo;
	function* fFrom;
	function* fTo;
	bool isBase;
	
	bool operator<(const process &other) const{
    	return (pFrom + pTo < other.pFrom + other.pTo);
    }
};

struct message{
	string pFrom;
	string pTo;
	function* fFrom;
	set<string>* params;
	int numParams;
	string order;
	string text;
	
	bool operator<(const message &other) const{
    	return (pFrom + pTo + order + text < other.pFrom + other.pTo + other.order + other.text);
    }

};

struct erlangInfo{
	string module;
	set<function> exports;
	set<function> imports;
	set<function> functions;
	set<functionCall> functionCalls;
	set<process> processes;
	set<message> messages;
};

void generateErlangInfo(AST *root, erlangInfo* erlangInfo);
