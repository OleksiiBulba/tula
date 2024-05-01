/*
 * MIT License
 *
 * Copyright (c) 2024 Alexey Kutepov <reximkut@gmail.com>
 * Copyright (c) 2024 Oleksii Bulba <oleksii.bulba@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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

