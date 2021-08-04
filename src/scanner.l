D			[0-9]
L			[a-zA-Z_]
IS          ((u|U)|(u|U)?(l|L|ll|LL)|(l|L|ll|LL)(u|U))


%{
    #include "atiling/yacc.h"
    #include <stdlib.h>
    #include <string.h>

    #include "atiling/macros.h"
    #include "atiling/fragment.h"
    #include "parser.h"

    void atiling_scanner_count();
    int  atiling_scanner(int, char*);
    void atiling_scanner_init();
    void atiling_scanner_copy();
    void atiling_scanner_pragma_begin(char **divs);
    void atiling_scanner_pragma_end();

    int scanner_pragma;           /**< Are we inside pragma? */
    int scanner_pragma_header;    /**< Are we parsing pragma header? */
    int scanner_line;             /**< Current line number */
    int scanner_column;           /**< Current column number */
    int scanner_column_LALR;      /**< Column number before token */

    int scanner_pragma_begin_col;        /**< Number of pragma */
    FILE *scanner_out;            /**< Tmp file where code is writter */
    atiling_fragment_p scanner_fragment;

%}

%option noyywrap
%option noinput
%option nounput

%%
"#pragma"[ ]+"trahrhe"[ ]"atiling"  {
                        if(scanner_fragment != NULL) {
                            scanner_fragment->id++;
                            return IGNORE; 
                        }
                        ATILING_debug("pragma atiling header start");
                        scanner_pragma_begin_col = scanner_column; 
                        atiling_scanner_count();
                        scanner_pragma_header = ATILING_TRUE;
                        return PRAGMA_BEGIN;
                    }
"#pragma"[ ]+"endtrahrhe" {
                        if(scanner_pragma == ATILING_FALSE)
                            return IGNORE;
                        ATILING_debug("pragma endatiling");
                        atiling_scanner_pragma_end();
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
    scanner_pragma_begin_col = 1;

    scanner_fragment = NULL;
}

/**
 * atiling_scanner_free function:
 * this function frees the memory allocated for the scanner. It frees
 * flex's buffer (it supposes there is only one buffer) since flex does
 * never free it itself.
 * WARNING: this is probably *not* portable...
 */
void atiling_scanner_free() {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    free(yy_buffer_stack);
    free(scanner_fragment);
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

    // If we are inside atiling pragma, we copy everything to the output file
    if(scanner_pragma) {
        fprintf(scanner_out, "%s", yytext);
    }
}

void atiling_scanner_pragma_begin(char **divs) {
    if(scanner_fragment != NULL) {
        return;
    }
    ATILING_debug("begin pragma");
    char name[ATILING_MAX_FILENAME_LEN] = {0};

    // output file name as loop.tmp
    // this file is expected to be parsed by clan
    if(snprintf(name, ATILING_MAX_FILENAME_LEN, "%s%s", 
                ATILING_PRAGMA_FILE_NAME, 
                ATILING_PRAGMA_FILE_EXT) < 0) {
        ATILING_error("snprintf");
    }

    scanner_out = fopen(name, "w");
    if(scanner_out == NULL) {
        ATILING_error("cannot open tmp output file");
    }
    
    scanner_fragment = atiling_fragment_malloc();
    scanner_fragment->divs = divs;

    ATILING_strdup(scanner_fragment->path, name);
    scanner_fragment->start[0] = scanner_line;
    scanner_fragment->start[1] 
                = scanner_pragma_begin_col;

    fprintf(scanner_out, "#pragma scop\n");

    scanner_pragma = ATILING_TRUE;
}

void atiling_scanner_pragma_end() {
    scanner_pragma = ATILING_FALSE;
    fprintf(scanner_out, "\n#pragma endscop\n");
    fclose(scanner_out);

    scanner_fragment->end[0] = scanner_line;
    scanner_fragment->end[1] = scanner_column;
    scanner_fragment->end[1] += strlen(yytext);


    ATILING_debug("end pragma");
}

int atiling_scanner(int token, char *message) {
    atiling_scanner_count();

    if (scanner_pragma_header == ATILING_TRUE) {
        if(token == I_CONSTANT || token == IDENTIFIER) {
            ATILING_strdup(yylval.str, yytext);
        }
        return token;        
    }
    else
        return IGNORE;
}