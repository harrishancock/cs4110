%option case-insensitive

%{

enum {
    eof,
    lparen = '(',
    rparen = ')',
    colon = ':',
    semicolon = ';'
    id = 257,
    numeric, string,
    boolean,
    type,
    assign, eof,
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
    assign,
    not
};

%}

[:space:]+          ;
"--".*\n            ;
"if"                { return if_; }
"then"              { return then; }
"is"                { return is; }
"while"             { return while_; }
"begin"             { return begin; }
"end"               { return end; }
"procedure"         { return procedure; }
"declare"           { return declare; }
"constant"          { return constant; }
"loop"              { return loop; }
"begin"             { return begin; }
"end"               { return end; }
"or"                { return addop; }
"and"               { return mulop; }
"mod"               { return mulop; }
"true"              { return boolean; }
"false"             { return boolean; }
"integer"           { return type; }
"boolean"           { return type; }
"real"              { return type; }
"not"               { return not_; }
[[:alpha:]]([[:alnum:]]|_[[:alnum:]])*  { return id; }
[[:digit:]]+("."[[:digit:]]+)?          { return numeric; }
"\""([^"\""]|"\"\"")*"\""               { return string; }
":="                { return assign; }
":"                 { return colon; }
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
