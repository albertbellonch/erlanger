#include <string>
#include <set>
#include <map>

using namespace std;

#define AST_FIELDS string kind;string text;
#define zzcr_ast(as,attr,ttkind,textt) as=new AST;as->kind=(attr)->kind;as->text=(attr)->text;as->line=(attr)->line;as->right=NULL;as->down=NULL;as->sc=0;as->tp=0;as->ref=0;
#define createASTlist (*_root)=new AST;((*_root))->kind="list";((*_root))->right=NULL;((*_root))->down=_sibling;

#include "ast.h"
#include "graphgen.hh"

string intToStr(int i){
	stringstream out;
	out << i;
	return out.str();
}

int strToInt(string s){
	return atoi(s.c_str());
}

bool isEqual(string a, string b){
	return (a.compare(b)==0);
}

function* generateFunction(string m, string s, int n){
	function *f = new function();
	f->module = m;
	f->name = s;
	f->params = n;
	return f;
}

function* getFunction(AST *ast, string defaultMod){
	// Lazy init
	AST *functionInfo = ast->down;
	function *funct = generateFunction("", functionInfo->text, 0);
	functionInfo = functionInfo->right;
	
	// Is it from a module?
	if(functionInfo!=NULL && isEqual(functionInfo->kind, "atom")){
		funct->module = funct->name;
		funct->name = functionInfo->text;
		functionInfo = functionInfo->right;
	}else{
		funct->module = defaultMod;
	}
	
	// Has params?
	if(functionInfo!=NULL && isEqual(functionInfo->kind, "params")){
		functionInfo = functionInfo->down;
		while(functionInfo!=NULL){
			funct->params++;
			functionInfo = functionInfo->right;
		}	
	}
	
	// Return
	return funct;
}

string getPidFromFunction(function* funct, set<process>& pcs){
	// Search if this function is called by a created process
	set<process>::iterator itpc;
	process pc;
	for(itpc = pcs.begin(); itpc != pcs.end(); itpc++ ) {
		pc = *itpc;
		if(isEqual(funct->name, pc.fTo->name)){
			// Found, return process PID
			return pc.pTo;
		}
	}
	
	// Return default: main process
	return "erlang";
}

string getValue(AST *ast){
	// Primary value
	if(isEqual(ast->kind,"int")
		||isEqual(ast->kind,"intB")
		||isEqual(ast->kind,"intC")
		||isEqual(ast->kind,"float")
		||isEqual(ast->kind,"floatexp")
		||isEqual(ast->kind,"var")
		||isEqual(ast->kind,"atom")) 
		return ast->text;
		
	// Function
	if(isEqual(ast->kind,"functionhead")){
		AST* fct = ast->down;
		string s = fct->text;
		fct = fct->right;
		s += "(";
		if(fct!=NULL){
			fct = fct->down;
			bool first = true;
			while(fct!=NULL){
				if(!first)
					s += ",";
				s += getValue(fct);
				fct = fct->right;
			}
		}
		s += ")";
		return s;
	}
		
	// List
	if(isEqual(ast->kind,"list")){
		return ast->text;
	}
	
	// Default
	return ast->kind;
}

void searchForFunctionCalls(AST *ast, function* funct, set<function>& fs, set<functionCall>& fcs, string defaultMod){
	// If null, do anything
	if(ast==NULL){
		return;
	}
	
	if(isEqual(ast->kind,"functionhead")){
		// Call found
		function *called = getFunction(ast, defaultMod);
		fs.insert(*called);
		functionCall *fc = new functionCall();
		fc->from = funct;
		fc->to = called;
		fcs.insert(*fc);
		
		// Place in params
		ast = ast->down;
		while((ast!=NULL)&&(!isEqual(ast->kind,"params"))) ast = ast->right;
		if(ast!=NULL) ast = ast->down;
		
		// Search in params
		while(ast!=NULL){
			searchForFunctionCalls(ast, funct, fs, fcs, defaultMod);
			ast = ast->right;
		}
	}else{
		// Search in sons
		ast = ast->down;
		while(ast!=NULL){
			searchForFunctionCalls(ast, funct, fs, fcs, defaultMod);
			ast = ast->right;
		}
	}	
}

int getPos(set<string>* variables, string possibleVar){
	set<string>::iterator itv;
	int i = 0;
	for(itv = variables->begin(); itv != variables->end(); itv++ ) {
		string var = *itv;
		if(isEqual(var, intToStr(i)+"/"+possibleVar))
			return i;
		i++;
	}
	return -1;
}

int getLength(set<string>* variables){
	set<string>::iterator itv;
	int i = 0;
	for(itv = variables->begin(); itv != variables->end(); itv++ ) {
		i++;
	}
	return i;
}

string getValue(set<string>* variables, int pos){
	set<string>::iterator itv;
	int i = 0;
	for(itv = variables->begin(); itv != variables->end(); itv++ ) {
		if(i==pos){
			string s = *itv;
			s = s.substr(s.find("/")+1, s.size());
			return s;
		}
		i++;
	}
	return NULL;
}

AST* treatSend(AST* processInstrBody, function* funct, set<process>& pcs, set<message>& ms, map<string,int>& savedMessages, set<string>* rcvParams){
	// Get PIDs
	string thispid = getPidFromFunction(funct, pcs);
	string pid = processInstrBody->text;
	
	// Check if it is a variable
	bool isVar = false;
	set<string> pids;
	if(rcvParams!=NULL){
		int paramPos = getPos(rcvParams,pid);
		int paramLength = getLength(rcvParams);
		isVar = (paramPos>=0);
		if(isVar){
			set<message>::iterator itm;
			message m;
			for(itm = ms.begin(); itm != ms.end(); itm++) {
				m = *itm;
				if(isEqual(m.pTo, thispid)&&(paramLength==m.numParams)){
					string possibleValue = getValue(m.params, paramPos);
					char firstChar = possibleValue[0];
					if((firstChar >= 'A')&&(firstChar <= 'Z')){
						pids.insert(possibleValue);
					}else if(isEqual(possibleValue, "self()")){
						pids.insert(m.pFrom);
					}
				}
			}
		}
	}
	
	// Get message
	processInstrBody = processInstrBody->right;
	string msgTxt;
	set<string>* params = new set<string>();
	int numParams = 0;
	if(isEqual(processInstrBody->kind,"envelope")){
		msgTxt = "{";
		AST *msgParams = processInstrBody->down;
		bool first = true;
		int i = 0;
		while(msgParams!=NULL){
			if(!first){
				msgTxt += ",";
			}
			string value = getValue(msgParams);
			params->insert(intToStr(i)+"/"+value);
			numParams++;
			msgTxt += value;
			first = false;
			msgParams = msgParams->right;
			i++;
		}
		msgTxt += "}";
	}else{
		msgTxt = processInstrBody->text;
	}
	
	// New message/s
	if(!isVar){
		savedMessages[pid]++;
		message *m = new message();
		m->pFrom = getPidFromFunction(funct, pcs);
		m->pTo = pid;
		m->fFrom = funct;
		m->params = params;
		m->numParams = numParams;
		if(rcvParams==NULL){
			m->order = intToStr(savedMessages[pid]);
		}else{
			m->order = "0";
		}
		m->text = msgTxt;
		ms.insert(*m);
	}else{
		set<string>::iterator itm;
		for(itm = pids.begin(); itm != pids.end(); itm++ ) {
			string pidd = *itm;
			message *m = new message();
			m->pFrom = getPidFromFunction(funct, pcs);
			m->pTo = pidd;
			m->fFrom = funct;
			m->params = params;
			m->numParams = numParams;
			m->order = "0";
			m->text = msgTxt;
			ms.insert(*m);
		}
	}
	
	return processInstrBody;
}

void searchForProcessesInfo(AST *ast, function* funct, set<process>& pcs, set<message>& ms, bool firstProcessInstr, map<string,int>& savedMessages){
	// If null, do anything
	if(ast==NULL){
		return;
	}
	
	// Treat each instruction type separately
	AST *processInstrBody = ast->down;
	
	if(isEqual(ast->kind,"spawn")){
		// SPAWN
		
		// New process var
		string varName = processInstrBody->text;
		
		// Module
		processInstrBody = processInstrBody->right;
		string module = processInstrBody->text;
		
		// Function
		processInstrBody = processInstrBody->right;
		string functionName = processInstrBody->text;
		
		// Args
		processInstrBody = processInstrBody->right;
		int numArgs = 0;
		AST *list = processInstrBody->down;
		while(list!=NULL){
			numArgs++;
			list = list->right;
		}
		
		// Save implicit "create" message
		savedMessages[varName] = 1;
		
		// New process call
		process *pc = new process();
		pc->pFrom = getPidFromFunction(funct, pcs);
		pc->pTo = varName;
		pc->fFrom = funct;
		pc->fTo = generateFunction(module, functionName, numArgs);
		pc->isBase = false;
		pcs.insert(*pc);
		
		// Go to following
		processInstrBody = processInstrBody->right;
	}else if(isEqual(ast->kind,"send")){
		// SEND
		processInstrBody = treatSend(processInstrBody, funct, pcs, ms, savedMessages, NULL);
		
		// Go to following
		processInstrBody = processInstrBody->right;
	}else if(isEqual(ast->kind,"receive")){
		// RECEIVE
		// Skip, we'll check it later
		while(processInstrBody!=NULL && isEqual(processInstrBody->kind,"received")){
			// Move to following
			processInstrBody = processInstrBody->right;
		}
	}else{
		// Unrecognized type
		return;
	}
	
	// If first process instruction, treat other instructions
	while(processInstrBody!=NULL){
		searchForProcessesInfo(processInstrBody, funct, pcs, ms, false, savedMessages);
		processInstrBody = processInstrBody->right;
	}
}

void searchForMessagesInReceive(AST *ast, function* funct, set<process>& pcs, set<message>& ms, map<string,int>& savedMessages){
	// If null, do anything
	if(ast==NULL){
		return;
	}
	
	// Treat each instruction type separately
	AST *processInstrBody = ast->down;
	
	if(isEqual(ast->kind,"spawn")){
		// SPAWN
		// Skip, already checked
		// Skipping var, module, function and args
		processInstrBody = processInstrBody->right;
		processInstrBody = processInstrBody->right;
		processInstrBody = processInstrBody->right;
		processInstrBody = processInstrBody->right;
	}else if(isEqual(ast->kind,"send")){
		// Skip pid and message
		processInstrBody = processInstrBody->right;
		// Go to following
		processInstrBody = processInstrBody->right;
	}else if(isEqual(ast->kind,"receive")){
		// RECEIVE
		while(processInstrBody!=NULL && isEqual(processInstrBody->kind,"received")){
			// Current "received"
			// Save params 
			set<string> params;
			AST* paramList = processInstrBody->down->down;
			int i = 0;
			while(paramList!=NULL){
				string txt = paramList->text;
				params.insert(intToStr(i)+"/"+txt);
				paramList = paramList->right;
			}
			
			// Move to function list
			AST* functionList = processInstrBody->down->right->down;
			while(functionList!=NULL){
				if(isEqual(functionList->kind,"send")){
					functionList = treatSend(functionList->down, funct, pcs, ms, savedMessages, &params);
				}
				functionList = functionList->right;
			}
			
			// Move to following
			processInstrBody = processInstrBody->right;
		}
	}else{
		// Unrecognized type
		return;
	}
	
	// If first process instruction, treat other instructions
	while(processInstrBody!=NULL){
		searchForMessagesInReceive(processInstrBody, funct, pcs, ms, savedMessages);
		processInstrBody = processInstrBody->right;
	}
}

void generateErlangInfo(AST *root, erlangInfo* erlangInfo){
	// Init
	AST *list = root->down;	
	
	// See if there is module definition
	if(isEqual(list->kind,"-module")){
		erlangInfo->module = list->down->text;
		list = list->right;
	}
	
	// See if there are exports
	if(isEqual(list->kind,"-export")){
		AST *exportList = list->down;
		while(exportList!=NULL){
			function* expFunction = new function();
			expFunction->module = "";
			expFunction->name = exportList->down->text;
			if(!isEqual(erlangInfo->module,"")){
				expFunction->module = erlangInfo->module;
			}
			expFunction->params = strToInt(exportList->down->right->text);
			erlangInfo->exports.insert(*expFunction);
			exportList = exportList->right;
		}
		list = list->right;
	}
	
	// See if there are imports
	if(isEqual(list->kind,"-import")){
		AST *importList = list->down;
		while(importList!=NULL){
			// Check type
			function* impFunction = new function();
			if(isEqual(importList->kind,"listitem")){
				AST *importListitem = importList->down;
				impFunction->name = "";
				bool first = true;
				while(importListitem!=NULL){
					if(!first){
						impFunction->name += "_";
					}
					impFunction->name += importListitem->text;
					first = false;
					importListitem = importListitem->right;
				}
				impFunction->params = -1;

			}else{
				impFunction->name = importList->down->text;
				impFunction->params = strToInt(importList->down->right->text);
			}
			erlangInfo->imports.insert(*impFunction);
			importList = importList->right;
		}
		list = list->right;
	}
	
	// Get first function
	AST *functionTr = NULL;
	if(list!=NULL){
		functionTr = list->down;
	}
	
	AST *functionTrBackup = functionTr;
	
	map<string,int> savedMessages; // saved messages for every pid; useful to establish order
			
	// Navigate through all functions
	while(functionTr!=NULL){
		// Parts of the function
		AST* functionlist = functionTr->down;
		bool first = true;
		
		// Navigate through all function cases
		do{
			// Function case
		
			// Get name
			function* newFunct = getFunction(functionlist, erlangInfo->module);
			if(first){
				// Base process: Erlang
				process *pc = new process();
				pc->pFrom = "";
				pc->pTo = "erlang";
				pc->fFrom = NULL;
				pc->fTo = newFunct;
				pc->isBase = true;
				erlangInfo->processes.insert(*pc);
			}
			erlangInfo->functions.insert(*newFunct);
			functionlist = functionlist->right;
			
			// Detect calls to other functions
			searchForFunctionCalls(functionlist, newFunct, erlangInfo->functions, erlangInfo->functionCalls, erlangInfo->module);
			
			// Detect processes info
			searchForProcessesInfo(functionlist, newFunct, erlangInfo->processes, erlangInfo->messages, true, savedMessages);
			
			// Continue
			first = false;
			functionlist = functionlist->right;
		} while(functionlist!=NULL);
		
		// Next function
		functionTr = functionTr->right;
	}
	
	// Navigate through all functions again: messages in receive blocks
	functionTr = functionTrBackup;
	while(functionTr!=NULL){
		// Parts of the function
		AST* functionlist = functionTr->down;
		
		// Navigate through all functions
		do{
			// Get name
			function* funct = getFunction(functionlist, erlangInfo->module);
			functionlist = functionlist->right;
			
			// Detect more processes info
			searchForMessagesInReceive(functionlist, funct, erlangInfo->processes, erlangInfo->messages, savedMessages);
			
			// Continue
			functionlist = functionlist->right;
		} while(functionlist!=NULL);
		
		// Next function
		functionTr = functionTr->right;
	}
}
