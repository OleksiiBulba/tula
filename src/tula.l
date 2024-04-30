%{
#include "tula.tab.h"
%}

%%

"tape" { return TAPE; }
"case" { return CASE; }
[0-9]+ {
    #if YYDEBUG
    printf("Number token: %s\n", yytext);
    #endif
    yylval.num = atoi(yytext);
    return NUMBER;
}
"->" {
    yylval.num = 1;
    return R_ARROW;
}
"<-" {
    yylval.num = -1;
    return L_ARROW;
}
[a-zA-Z_][a-zA-Z0-9_]* {
    #if YYDEBUG
    printf("Identifier token: %s\n", yytext);
    #endif
    yylval.str = strdup(yytext);
    return IDENTIFIER;
}
[ \t]+ { /* Ignore white space. */ }
[\n]+ { return NEWLINE; }
. {
    #if YYDEBUG
    printf("Unexpected token: %s\n", yytext);
    #endif
    return yytext[0];
}

%%

int yywrap(void)
{
    return 1;
}

