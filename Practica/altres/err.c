/*
 * A n t l r  S e t s / E r r o r  F i l e  H e a d e r
 *
 * Generated from: examen0.g
 *
 * Terence Parr, Russell Quong, Will Cohen, and Hank Dietz: 1989-2001
 * Parr Research Corporation
 * with Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 */

#define ANTLR_VERSION	13333
#include "pcctscfg.h"
#include "pccts_stdio.h"

#include <string> 
#include <iostream>

using namespace std; 

typedef struct {
  string kind;	string text; 
} Attrib;

void zzcr_attr(Attrib *attr, int type, char *text);

#define AST_FIELDS string kind; string text; 
#define zzcr_ast(as, attr, ttype, textt) as = new AST;\
(as)->kind = (attr)->kind; (as)->text = (attr)->text;\
(as)->right = NULL; (as)->down = NULL; 
#define createASTlist(name) (*_root)=new AST;((*_root))->kind=name;((*_root))->right=NULL;((*_root))->down=_sibling;
#define zzSET_SIZE 4
#include "antlr.h"
#include "ast.h"
#include "tokens.h"
#include "dlgdef.h"
#include "err.h"

ANTLRChar *zztokens[5]={
	/* 00 */	"Invalid",
	/* 01 */	"@",
	/* 02 */	"SPACE",
	/* 03 */	"VARi",
	/* 04 */	"ATOMi"
};
SetWordType setwd1[5] = {0x0,0x1,0x0,0x0,0x0};
