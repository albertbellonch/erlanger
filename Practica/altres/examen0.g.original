#header
<<
#include "charptr.h"
>>
<<
#include "charptr.c"
int main() {
ANTLR(expr(), stdin);
}
>>
#lexclass START
#token SPACE "[\ \n]" << zzskip(); >>
#token VARi "[A-Z][a-zA-Z0-9\_]*"
#token ATOMi "[a-z][a-zA-Z0-9\_@]*"


expr: (VARi ATOMi)* "@";
