%{
#include "calc.tab.h"
#undef YY_INPUT
#define YY_INPUT(b, r, s) read_input(b, &r, s)
%}

%%
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
0x[a-f0-9]+ { yylval = strtol(yytext, NULL, 16); return NUMBER; }
\n          { return EOL; }
"|"         { return ABS; }
"("         { return OP;  }
")"         { return CP;  }
"//".* 
[ \t]
.           { printf("Mystery character %c\n", *yytext); }
%%

