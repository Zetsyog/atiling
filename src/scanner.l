D			[0-9]
L			[a-zA-Z_]
IS          ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))


%{
    #include <stdlib.h>
    #include <string.h>

    #include "atiling/macros.h"
    #include "src/parser.h"

    void atiling_scanner_count();
    int  atiling_scanner(int, char*);
    void atiling_scanner_initialize();
    void atiling_scanner_copy();
    void atiling_scanner_pragma_begin();

    int scanner_pragma;           /**< Are we inside pragma? */
    int scanner_pragma_header;    /**< Are we parsing pragma header? */
    int scanner_line;             /**< Current line number */
    int scanner_column;           /**< Current column number */
    int scanner_column_LALR;      /**< Column number before token */

%}

%option noyywrap
%option noinput
%option nounput

%%
"#pragma"[ ]+"atiling"  {
                        if (ATILING_DEBUG)
                            ATILING_debug("pragma atiling header start");

                        atiling_scanner_count();
                        scanner_pragma_header = ATILING_TRUE;
                        return PRAGMA_BEGIN;
                    }
"#pragma"[ ]+"endatiling" {
                        if (ATILING_DEBUG)
                            ATILING_debug("pragma endatiling");
                        
                        scanner_pragma = ATILING_FALSE;
                        atiling_scanner_count();
                        return PRAGMA_END;
                    }

{L}({L}|{D})*		{ return atiling_scanner(IDENTIFIER, "identifier"); }
[1-9]{D}*{IS}?	    { return atiling_scanner(I_CONSTANT, "int const"); }
"("                 { return atiling_scanner('(', "("); }
")"                 { return atiling_scanner(')', ")"); }
","                 { return atiling_scanner(',', ","); }
[ \t\v\n\f]         { atiling_scanner_count(); }
.                   { atiling_scanner_count(); }


%%

/**
 * atiling_scanner_init function:
 * this function initialises the scanner global variables with default values.
 */
void atiling_scanner_init() {
    yy_buffer_stack_top = 0;
    yy_buffer_stack = 0;
    yy_flush_buffer(YY_CURRENT_BUFFER);
    scanner_pragma = ATILING_FALSE;
    scanner_pragma_header = ATILING_FALSE;
    scanner_column = 1;
    scanner_line = 1;
    scanner_column_LALR = 1;
}

/**
 * atiling_scanner_count function:
 * this function updates the current line and column the scanner is reading.
 */
void atiling_scanner_count() {
    int i;

    scanner_column_LALR = scanner_column;
    for (i = 0; yytext[i] != '\0'; i++) {
        if (yytext[i] == '\n') {
            scanner_line++;
            scanner_column = 1;
        } else {
            scanner_column++;
        }
    }
}

void atiling_scanner_pragma_begin() {
    scanner_pragma = ATILING_TRUE;
    
}

int atiling_scanner(int token, char *message) {
    if (ATILING_DEBUG)
        fprintf(stderr, "[ATILING] Debug: Lex yytext = \"%s\" "
                        "(\"%s\" at line %d, column %d)\n",
                        yytext, message, scanner_line, scanner_column);

    atiling_scanner_count();

    if (scanner_pragma_header == ATILING_TRUE)
        return token;
    else
        return IGNORE;
}