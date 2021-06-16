%{
    #include "atiling/yacc.h"
    #include <stdlib.h>
    #include <stdio.h>

    #include "atiling/macros.h"
    #include "atiling/options.h"
    #include "atiling/pragma.h"

    extern FILE* yyin;                     /**< File to be read by Lex */
    extern int   scanner_pragma;           /**< Are we inside pragma? */
    extern int   scanner_pragma_header;    /**< Are we parsing pragma header? */
    extern int   scanner_line;
    extern int   scanner_column;
    extern atiling_pragma_p *scanner_pragmas;

    int  yylex(void);
    void yyerror(char*);
    void yyrestart(FILE*);
    void atiling_scanner_init();
    void atiling_scanner_pragma_begin(char **);

    char *input_name;
%}
%union {
    int value; 
    char *str;
    struct {
        int size;
        char **values;
    } str_array;
}

%token <str> I_CONSTANT IDENTIFIER

%token IGNORE PRAGMA_BEGIN PRAGMA_END

%type<str_array> arg_list
%type<str> arg

%start list
%%
list:
      pragma      { ATILING_debug("rule list.1: pragma"); }
    | list pragma { ATILING_debug("rule list.2: list pragma"); }
    | list IGNORE {}
    | IGNORE      {}
    ;

pragma:
      pragma_begin instr_list pragma_end 
    ;

pragma_begin:
      PRAGMA_BEGIN '(' arg_list ')' { 
            scanner_pragma_header = ATILING_FALSE;
            atiling_scanner_pragma_begin($3.values);
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
      arg { 
            ATILING_malloc($$.values, char **, sizeof(char *) * 2);
            $$.values[0] = $1;
            $$.values[1] = NULL;
            $$.size = 1;
            if(ATILING_DEBUG) {
                fprintf(stderr, "[ATILING] Debug: new arg_list size=%i\n", $$.size);
            }
        }
    | arg_list ',' arg {
            
            ATILING_realloc($1.values, char **, sizeof(char *) * ($1.size + 2));
            $$.size = $1.size + 1;
            $$.values = $1.values;
            $$.values[$$.size - 1] = $3;
            $$.values[$$.size] = NULL;
            if(ATILING_DEBUG) {
                fprintf(stderr, "[ATILING] Debug: arg_list size=%i\n", $$.size);
            }
        }
    ;

arg:
      I_CONSTANT { $$ = $1; }
    | IDENTIFIER { $$ = $1; }
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

atiling_pragma_p *atiling_parse(FILE *input, atiling_options_p options) {
    yyin = input;
    input_name = options->name;

    atiling_scanner_init();

    yyparse();

    ATILING_debug("parsing done");

    return scanner_pragmas;
}