#include <string>
#include <set>
#include <iostream>
#include <fstream>

using namespace std;

#include "ast.h"
#include "graphwriter.hh"
#include "graphgen.hh"

void writeLine(ofstream* file, string data, bool indent){
	if(indent){
		*file << "\t";	
	}
	*file << data << endl;
}

string getFunctionValue(function* function){
	string s = function->name;
	if(function->params>=0){
		stringstream params;
		params << function->params;
		s += "_" + params.str();
	}
	return s;
}

void writeFile(erlangInfo* erlangInfo, bool showModules, bool showExports, bool showImports, bool showFunctions, bool showProcesses){
	// Open file
	ofstream file;
	file.open("output/graph.dot");
	writeLine(&file, "digraph G{", false);
	
	// Module
	if(showModules && erlangInfo->module.compare("")!=0){
		writeLine(&file, "node [color=purple]", true);
		writeLine(&file, erlangInfo->module, true);
	}
	
	// Exports
	if(showExports && erlangInfo->exports.size()>0){
		writeLine(&file, "node [color=green]", true);
		writeLine(&file, "edge [color=green]", true);
		writeLine(&file, "export", true);
		set<function>::iterator ite;
		function exprtFunc;
		writeLine(&file, "node [color=blue]", true);
			for(ite = erlangInfo->exports.begin(); ite != erlangInfo->exports.end(); ite++ ) {
			exprtFunc = *ite;
			writeLine(&file,  "export -> " + getFunctionValue(&exprtFunc), true);
		} 
	}
	
	// Imports
	if(showImports && erlangInfo->imports.size()>0){
		writeLine(&file, "node [color=orange]", true);
		writeLine(&file, "edge [color=orange]", true);
		writeLine(&file, "import", true);
		set<function>::iterator iti;
		function imprtFunc;
		writeLine(&file, "node [color=blue]", true);
		for(iti = erlangInfo->imports.begin(); iti != erlangInfo->imports.end(); iti++ ) {
			imprtFunc = *iti;
			writeLine(&file, "import -> " + getFunctionValue(&imprtFunc), true);
		}
	} 
	
	// Function names
	if(showModules)
		writeLine(&file, "edge [color=purple]", true);
	set<string> printedFCalls; // printed call functions
	set<function>::iterator itf;
	function func;
	for(itf = erlangInfo->functions.begin(); itf != erlangInfo->functions.end(); itf++ ) {
		func = *itf;
		if(showFunctions){
			writeLine(&file, "node [color=blue]", true);
			writeLine(&file, getFunctionValue(&func), true);
		}
		if(showModules && func.module.compare(erlangInfo->module)!=0){
			writeLine(&file, "node [color=purple]", true);
			writeLine(&file, func.module, true);
			if(showFunctions)
				writeLine(&file, func.module + " -> " + getFunctionValue(&func), true);
		}
	} 
	
	// Function calls
	if(showFunctions){
		writeLine(&file, "edge [color=blue]", true);
		set<functionCall>::iterator itfc;
		functionCall fc;
		for(itfc = erlangInfo->functionCalls.begin(); itfc != erlangInfo->functionCalls.end(); itfc++ ) {
			fc = *itfc;
			string call = getFunctionValue(fc.from) + " -> " + getFunctionValue(fc.to);
			if(printedFCalls.count(call)==0){
				writeLine(&file, call, true);
				printedFCalls.insert(call);
			}
		} 
	}
	
	// Process calls
	if(showProcesses){
		// Process creations
		writeLine(&file, "node [color=red]", true);
		writeLine(&file, "erlang", true);
		set<string> printedPCalls; // printed process calls
		set<process>::iterator itpc;
		process pc;
		for(itpc = erlangInfo->processes.begin(); itpc != erlangInfo->processes.end(); itpc++ ) {
			pc = *itpc;
			writeLine(&file, pc.pTo, true);
			writeLine(&file, "edge [color=red]", true);
			if(showFunctions){
				if(!pc.isBase){
					writeLine(&file, getFunctionValue(pc.fFrom) + " -> " + pc.pTo + " [label=\"1. create\"]", true);
					writeLine(&file, "edge [color=blue]", true);
					if(printedPCalls.count(pc.fFrom->name)==0){
						writeLine(&file, pc.pFrom + " -> " + getFunctionValue(pc.fFrom), true);
						printedPCalls.insert(pc.fFrom->name);
					}
				}
				writeLine(&file, "edge [color=blue]", true);
				if(printedPCalls.count(pc.fTo->name)==0){
					writeLine(&file, pc.pTo + " -> " + getFunctionValue(pc.fTo), true);
					printedPCalls.insert(pc.fTo->name);
				}
			}else{
				if(!pc.isBase)
					writeLine(&file, pc.pFrom + " -> " + pc.pTo + " [label=\"1. create\"]", true);
			}
		}
		
		// Messages
		set<message>::iterator itm;
		message m;
		for(itm = erlangInfo->messages.begin(); itm != erlangInfo->messages.end(); itm++ ) {
			m = *itm;
			writeLine(&file, pc.pTo, true);
			writeLine(&file, "edge [color=red]", true);
			string label = m.text;
			if(m.order.compare("0")) label = m.order + ". " + label;
			if(showFunctions){
				writeLine(&file, getFunctionValue(m.fFrom) + " -> " + m.pTo + " [label=\"" + label + "\"]", true);
			}else{
				writeLine(&file, m.pFrom + " -> " + m.pTo + " [label=\"" + label + "\"]", true);
			}
		}
	}
		
	// Close file
	file << "}" << endl;
	file.close();
}

// GRAPH GENERATION
void generateGraph(AST *root, bool showModules, bool showExports, bool showImports, bool showFunctions, bool showProcesses){
	// Init
	erlangInfo erlangInfo;
	
	// Generate info
	cout << "Generating graph model… " << endl;
	generateErlangInfo(root, &erlangInfo);
	
	// Write to dot file
	cout << "Drawing graph… " << endl;
	writeFile(&erlangInfo, showModules, showExports, showImports, showFunctions, showProcesses);
	
	// Create graph and delete dot file
	system("dot -Tpdf output/graph.dot -o output/graph.pdf");
	system("rm output/graph.dot");
}