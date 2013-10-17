%option case-insensitive
%option yylineno

%{

#include <stdio.h>

enum {
    eof,
    lparen = '(',
    rparen = ')',
    colon = ':',
    semicolon = ';',
    id = 257,
    numeric, string,
    boolean,
    type,
    assign,
    mulop, addop, relop,
    if_,
    while_,
    then,
    is,
    procedure,
    declare,
    constant,
    loop,
    begin,
    end,
    write_,
    read_,
    not_
};

%}

%%
\n                  { } /* Need separate rule for yylineno to increment. */
[ \r\t]+          ;
"--".*            ;
"begin"             { return begin; }
"end"               { return end; }
"if"                { return if_; }
"then"              { return then; }
"is"                { return is; }
"while"             { return while_; }
"procedure"         { return procedure; }
"declare"           { return declare; }
"constant"          { return constant; }
"loop"              { return loop; }
"or"                { return addop; }
"and"               { return mulop; }
"mod"               { return mulop; }
"true"              { return boolean; }
"false"             { return boolean; }
"integer"           { return type; }
"boolean"           { return type; }
"real"              { return type; }
"not"               { return not_; }
"put"               { return write_; }
"put_line"          { return write_; }
"get"               { return read_; }
[[:alpha:]]([[:alnum:]]|_[[:alnum:]])*  { return id; }
[[:digit:]]+("."[[:digit:]]+)?          { return numeric; }
"\""([^"\""]|"\"\"")*"\""               { return string; }
":"                 { return colon; }
":="                { return assign; }
";"                 { return semicolon; }
"+"                 { return addop; }
"-"                 { return addop; }
"*"                 { return mulop; }
"/"                 { return mulop; }
[=<>]               { return relop; }
"("                 { return lparen; }
")"                 { return rparen; }
<<EOF>>             { return eof; }
%%

const char* token_to_string (int tok) {
    switch (tok) {
#define ENUMITEM(x) case x: return #x;
        ENUMITEM(eof)
        ENUMITEM(lparen)
        ENUMITEM(rparen)
        ENUMITEM(colon)
        ENUMITEM(semicolon)
        ENUMITEM(id)
        ENUMITEM(numeric)
        ENUMITEM(string)
        ENUMITEM(boolean)
        ENUMITEM(type)
        ENUMITEM(assign)
        ENUMITEM(mulop)
        ENUMITEM(addop)
        ENUMITEM(relop)
        ENUMITEM(if_)
        ENUMITEM(while_)
        ENUMITEM(then)
        ENUMITEM(is)
        ENUMITEM(procedure)
        ENUMITEM(declare)
        ENUMITEM(constant)
        ENUMITEM(loop)
        ENUMITEM(begin)
        ENUMITEM(end)
        ENUMITEM(write_)
        ENUMITEM(read_)
        ENUMITEM(not_)
#undef ENUMITEM
        default:
            return "(unknown token)";
    }
}

int main () {
    int t;
    while (t = yylex()) {
        printf("%d: %s is \"%s\"\n", yylineno, token_to_string(t), yytext);
    }
}
