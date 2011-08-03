/*
 * A n t l r  T r a n s l a t i o n  H e a d e r
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * With AHPCRC, University of Minnesota
 * ANTLR Version 1.33MR33
 *
 *   antlr -gt examen0.g
 *
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
#define GENAST

#include "ast.h"

#define zzSET_SIZE 4
#include "antlr.h"
#include "tokens.h"
#include "dlgdef.h"
#include "mode.h"

/* MR23 In order to remove calls to PURIFY use the antlr -nopurify option */

#ifndef PCCTS_PURIFY
#define PCCTS_PURIFY(r,s) memset((char *) &(r),'\0',(s));
#endif

#include "ast.c"
zzASTgvars

ANTLR_INFO

#include <cstdlib> 
#include <cmath>

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
    case ATOMi:
    attr->kind="atom";
    attr->text=text;
    break;
    case VARi:
    attr->kind="var";
    attr->text=text;
    break;
    default:
    attr->kind=lowercase(text);
    attr->text="";
    break;
  }
}
int main() {
  AST *root = NULL; 
  ANTLR(expr(&root), stdin); 
}

void
#ifdef __USE_PROTOS
expr(AST**_root)
#else
expr(_root)
AST **_root;
#endif
{
  zzRULE;
  zzBLOCK(zztasp1);
  zzMake0;
  {
  {
    zzBLOCK(zztasp2);
    zzMake0;
    {
    while ( (LA(1)==VARi) ) {
      zzmatch(VARi); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzmatch(ATOMi); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
      zzLOOP(zztasp2);
    }
    zzEXIT(zztasp2);
    }
  }
  zzmatch(1); zzsubchild(_root, &_sibling, &_tail); zzCONSUME;
  zzEXIT(zztasp1);
  return;
fail:
  zzEXIT(zztasp1);
  zzsyn(zzMissText, zzBadTok, (ANTLRChar *)"", zzMissSet, zzMissTok, zzErrk, zzBadText);
  zzresynch(setwd1, 0x1);
  }
}
