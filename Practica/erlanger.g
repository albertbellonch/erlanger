#header 

<< 
#include <string> 
#include <iostream>
#include <map>

using namespace std; 

typedef struct {
	string kind;	string text; 
} Attrib;

void zzcr_attr(Attrib *attr, int type, char *text);

#define AST_FIELDS string kind; string text; 
#define zzcr_ast(as, attr, ttype, textt) as = new AST;\
	(as)->kind = (attr)->kind; (as)->text = (attr)->text;\
	(as)->right = NULL; (as)->down = NULL; 
#define createASTlist(name) #0=new AST;(#0)->kind=name;(#0)->right=NULL;(#0)->down=_sibling;
>>

<< 
#include <cstdlib> 
#include <cmath>
#include "graphwriter.hh"

bool getFlagValue(string flags, char f){
	int fo = flags.find_first_of(f);
	bool minZero = fo>=0;
	bool maxLen = fo<flags.length();
	bool val = minZero && maxLen;
	return val;
}

string lowercase(string s)
{
  for (int i=0;i<(int)s.size();i++) {
    if (s[i]>='A' && s[i]<='Z') {
      s[i]=s[i]-'A'+'a';
    }
  }
  return s;
}

void zzcr_attr(Attrib *attr, int type, char *text) { 
	switch (type) {
	case NAT:
	case NINT:
		attr->kind="int";
		attr->text=text;
		break;
	case INTBX:
		attr->kind="intB";
		attr->text=text;
		break;
	case INTCHAR:
		attr->kind="intC";
		attr->text=text;
		break;
	case FLTP:
	case NFLTP:
		attr->kind="float";
		attr->text=text;
		break;
	case FLTPEXP:
	case NFLTPEXP:
		attr->kind="floatexp";
		attr->text=text;
		break;
	case VAR:
		attr->kind="var";
		attr->text=text;
		break;
	case ATOM:
	case QATOM:
		attr->kind="atom";
		attr->text=text;
		break;
	case LIST:
		attr->kind="list";
		attr->text=text;
		break;
	default:
		attr->kind=lowercase(text);
		attr->text="";
		break;
	}
}

void printHelp(){
	cout << "Help: " << endl;
	cout << "- An Erlang source code file must be provided as a standard input" << endl;
	cout << "- A graph representing that Erlang file will be generated at 'ouput/graph.pdf'" << endl;
	cout << "- Program options: " << endl;
	cout << "	- h: Print this information" << endl;
	cout << "	- t: Print Erlang tree" << endl;
	cout << "	- m: Show modules in the graph" << endl;
	cout << "	- e: Show exports in the graph" << endl;
	cout << "	- i: Show imports in the graph" << endl;
	cout << "	- f: Show functions in the graph" << endl;
	cout << "	- p: Show processes in the graph" << endl;
	cout << "	- DEFAULT: all options enabled" << endl;
}
void ASTPrintIndent(AST *a, string s) { 
	if (a == NULL) return; 
	cout << s << " " << a->kind; 
	if (a->text != "") cout << "(" << a->text << ")"; 
	cout << endl;	ASTPrintIndent(a->down, s + " |"); 
	ASTPrintIndent(a->right, s);}
void ASTPrint(AST *a) { 
	ASTPrintIndent(a, "");
}
int main(int argc, char *argv[]) {
	// Hello
	cout << "-----ERLANGER-----" << endl;
	
	// Init flags with true
	bool showTree = true;
	bool showModules = true;
	bool showExports = true;
	bool showImports = true;
	bool showFunctions = true;
	bool showProcesses = true;
	
	// Put value to the flags
	if (argc==2){
		string flags(argv[1]);
		if(flags[0]=='-'){
			string flagsContent = flags.substr(1);
			if(getFlagValue(flagsContent, 'h')){
				printHelp();
				return 0;
			}
			showTree = getFlagValue(flagsContent, 't');
			showModules = getFlagValue(flagsContent, 'm');
			showExports = getFlagValue(flagsContent, 'e');
			showImports = getFlagValue(flagsContent, 'i');
			showFunctions = getFlagValue(flagsContent, 'f');
			showProcesses = getFlagValue(flagsContent, 'p');
		}
	}

	// Analyze Erlang file
	AST *root = NULL; 
	cout << "Analyzing Erlang file… " << endl;
	ANTLR(program(&root), stdin); 
	
	// Show tree if requested
	if(showTree){
		cout << "Showing Erlang tree:" << endl << endl;		ASTPrint(root);
		cout<<endl;
	}else{
		cout << "Erlang tree not shown as requested" << endl;
	}
	
	// Show result
	if (zzLexErrCount>0) {
		cout<<"There were lexical errors.";
	} else if (zzSyntaxErrCount>0) {
		cout<<"There were syntax errors.";
	} else {
		generateGraph(root, showModules, showExports, showImports, showFunctions, showProcesses);
    	cout<<"A graph representing the Erlang source code is available in file 'output/graph.pdf'";
	}
	cout<<endl;}

>>

#lexclass START
#token MOD			"\-module"
#token EXP			"\-export"
#token IMP			"\-import"
#token SPAWN		"spawn"
#token RECEIVE		"receive"
#token END			"end"
#token NAT	 		"[0-9]+" 
#token NINT			"\-[0-9]+"
#token INTBX 		"[0-9]+#[0-9A-Z]+" 
#token INTCHAR	 	"\$[a-zA-Z]+"
#token FLTP			"[0-9]+\.[0-9]*"
#token NFLTP		"\-[0-9]+\.[0-9]*"
#token FLTPEXP		"[0-9]+\.[0-9]*[eE][\-\+][0-9]+"
#token NFLTPEXP		"\-[0-9]+\.[0-9]*[eE][\-\+][0-9]+"
#token VAR	 		"[A-Z][a-zA-Z0-9\_@]*"
#token ATOM 		"[a-z][a-zA-Z0-9\_@]*"
#token QATOM 		"\'[a-zA-Z0-9\_\ \\\.\/\:@]*\'"
#token LIST			"\"~[\"]*\""
#token EQUAL        "\="
#token PLUS         "\+"
#token MINUS        "\-"
#token DOT          "\."
#token SEMICOLON	"\;"
#token COLON	  	"\:"
#token COMMA		"\,"
#token ASTERISK		"\*"
#token SLASH		"\/"
#token FUNCDEF		"\-\>"
#token OPENPARENT	"\("
#token CLOSEPARENT 	"\)"
#token OPENSQUARE 	"\["
#token CLOSESQUARE	"\]"
#token OPENCURLY 	"\{"
#token CLOSECURLY 	"\}"
#token VERTICALBAR 	"\|"
#token EXCLAMATION 	"\!"
#token SPACE 		"[\ \n\t]" << zzskip();>>
#token COMMENT      "\%~[\n]*" << zzskip();>>

program: (module | ) (exports | ) (imports | ) functions <<createASTlist("erlang")>> ;

module: MOD^ OPENPARENT! atom CLOSEPARENT! DOT!;
exports: EXP! OPENPARENT! OPENSQUARE! exportt (COMMA! exportt)* CLOSESQUARE! CLOSEPARENT! DOT! <<createASTlist("-export")>> ;
exportt: atom SLASH! nat <<createASTlist("item")>> ;
imports: IMP! OPENPARENT! importt (COMMA! importt)* CLOSEPARENT! DOT! <<createASTlist("-import")>> ;
importt: ( importold | importnew );
importold: atom (DOT! atom)* <<createASTlist("listitem")>> ;
importnew: OPENSQUARE! importitem (COMMA! importitem)* CLOSESQUARE!;
importitem:  atom SLASH! nat <<createASTlist("normalitem")>> ;

functions: (function)* <<createASTlist("functions")>>;
function: functionhead FUNCDEF! functionbody DOT! <<createASTlist("function")>>;

functionhead: atom (COLON! atom | ) OPENPARENT! (functionparams | ) CLOSEPARENT! <<createASTlist("functionhead")>>;
functionparams:  functionparam (COMMA! functionparam)* <<createASTlist("params")>> ;
functionparam: expr ;

functionbody:  
	var 
		( 
			( ( ASTERISK^ | SLASH^ ) value)* ( ( PLUS^ | MINUS^ ) term)* moresequential
		| 
			(
				EQUAL! SPAWN^ OPENPARENT! atom COMMA! atom COMMA! list CLOSEPARENT! 
			| 
				EXCLAMATION! msgcontent <<createASTlist("send")>>
			) 
			(COMMA! concurrentinstr)*
		) 
	| 
		valuenotvar ( ( ASTERISK^ | SLASH^ ) value)* ( ( PLUS^ | MINUS^ ) term)* moresequential
	| 
		RECEIVE! receivebody END! (COMMA! concurrentinstr)* ;

concurrentinstr: 
	var 
		(
			EQUAL! SPAWN^ OPENPARENT! atom COMMA! atom COMMA! list CLOSEPARENT! 
		| 
			EXCLAMATION! msgcontent <<createASTlist("send")>>
		)
	| 
		RECEIVE! receivebody END!;
		
moresequential: (SEMICOLON! functionhead FUNCDEF! expr)* ;
		
msgcontent: ( atom |  msgenvelope );
msgenvelope: OPENCURLY! valuewithatom (COMMA! valuewithatom)* CLOSECURLY! <<createASTlist("envelope")>>;
receivebody: received (SEMICOLON! received)* <<createASTlist("receive")>>;
received: msgcontent FUNCDEF! receivedfunctions <<createASTlist("received")>>;
receivedfunctions: receivedfunction (COMMA! receivedfunction)* <<createASTlist("receivedfunctions")>>;
receivedfunction: ( functionheadoratom  <<createASTlist("plainfunction")>> | var EXCLAMATION! msgcontent <<createASTlist("send")>>);
		
expr:  term ( ( PLUS^ | MINUS^ ) term)*;
exprwithfirstseen: ( ( ASTERISK^ | SLASH^ ) value)* ( ( PLUS^ | MINUS^ ) term)* ;
exprwithoutvar: valuenotvar exprwithfirstseen ;
term: value ( ( ASTERISK^ | SLASH^ ) value)* ;

value: var | valuenotvar;
valuenotvar: functionhead | number | tuple | list ;
valuewithatom: var | functionheadoratom | number | tuple | list ;
functionheadoratom: ATOM ( | (COLON! ATOM | ) OPENPARENT! (functionparams | ) CLOSEPARENT! <<createASTlist("functionhead")>>) ;
tuple: OPENCURLY! (items | ) CLOSECURLY! <<createASTlist("tuple")>> ;
list: OPENSQUARE! ( | value ( (VERTICALBAR! items) | (COMMA! items ) | ) ) CLOSESQUARE! <<createASTlist("list")>> | LIST ;
items: valuewithatom (COMMA! valuewithatom)* ;

var : VAR ;
number: integer | floatingp ;
nat: NAT ;
integer: nat | NEGINT | INTBX | INTCHAR ;
floatingp: FLTP | NFLTP | FLTPEXP | NFLTPEXP;
atom: ATOM | QATOM ;
