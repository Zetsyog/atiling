%{
    #include <stdlib.h>
    #include <stdio.h>

    #include "atiling/macros.h"
    #include "atiling/options.h"

    extern FILE* yyin;                     /**< File to be read by Lex */
    extern int   scanner_pragma;           /**< Are we inside pragma? */
    extern int   scanner_pragma_header;    /**< Are we parsing pragma header? */
    extern int   scanner_line;
    extern int   scanner_column;

    int  yylex(void);
    void yyerror(char*);
    void yyrestart(FILE*);
    void atiling_scanner_init();

    char *input_name;


%}

%token I_CONSTANT IDENTIFIER
;

%token IGNORE PRAGMA_BEGIN PRAGMA_END

%start list
%%
list:
      pragma      { ATILING_debug("rule list.1: pragma"); }
    | list pragma { ATILING_debug("rule list.2: list pragma"); }
    | list IGNORE { ATILING_debug("rule list.3: list IGNORE"); }
    | IGNORE      { ATILING_debug("rule list.4: IGNORE"); }
    ;

pragma:
      pragma_begin instr_list pragma_end 
    ;

pragma_begin:
      PRAGMA_BEGIN '(' arg_list ')' { 
            scanner_pragma_header = ATILING_FALSE;
            scanner_pragma = ATILING_TRUE;
        }
    ;

pragma_end:
      PRAGMA_END { 
            scanner_pragma = ATILING_FALSE;
        }
    ;

instr_list:
      instr_list IGNORE
    | IGNORE
    ;


arg_list:
      arg
    | arg_list ',' arg
    ;

arg:
      I_CONSTANT
    | IDENTIFIER
    ;



%%

void yyerror(char *s) {
	int i, line = 1;
	char c = 'C';
	FILE *file;

	ATILING_debug("parse error notified");

    fprintf(stderr, "[ATILING] Error: %s at line %d, column %d.\n",
            s, scanner_line, scanner_column - 1);

    file = fopen(input_name, "r");
    if (file != NULL) {
        // Go to the right line.
        while (line != scanner_line) {
            c = fgetc(file);
            if (c == '\n')
                line++;
        }

        // Print the line.
        while (c != EOF) {
            c = fgetc(file);
            fprintf(stderr, "%c", c);
            if (c == '\n')
                break;
        }

        // Print the situation line.
        for (i = 0; i < scanner_column - 1; i++) {
            if (i < scanner_column - 5)
                fprintf(stderr, " ");
            else if (i < scanner_column - 2)
                fprintf(stderr, "~");
            else
                fprintf(stderr, "^\n");
        }
        fclose(file);
    } else {
        ATILING_warning("cannot open input file");
    }
}

void atiling_parse(FILE *input, atiling_options_p options) {
    yyin = input;
    input_name = options->name;

    atiling_scanner_init();

    yyparse();

    ATILING_debug("parsing done");
}