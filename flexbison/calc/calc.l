%{
#include "calc.tab.h"
%}

%%
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
\n          { return EOL; }
"|"         { return ABS; }
"("         { return OP;  }
")"         { return CP;  }
"//".* 
[ \t]
.           { printf("Mystery character %c\n", *yytext); }
%%

