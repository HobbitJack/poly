%option noyywrap
%option header-file="poly.l.h"

%%
[[:graph:]]+	{ return 0; }
[\n]	{ return 1; }

[ \t]	{ /* ignore whitespace */ }
<<EOF>>	{ return -1; }
%%
