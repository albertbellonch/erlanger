#ifndef tokens_h
#define tokens_h
/* tokens.h -- List of labelled tokens and stuff
 *
 * Generated from: examen0.g
 *
 * Terence Parr, Will Cohen, and Hank Dietz: 1989-2001
 * Purdue University Electrical Engineering
 * ANTLR Version 1.33MR33
 */
#define zzEOF_TOKEN 1
#define SPACE 2
#define VARi 3
#define ATOMi 4

#ifdef __USE_PROTOS
void expr(AST**_root);
#else
extern void expr();
#endif

#endif
extern SetWordType setwd1[];
