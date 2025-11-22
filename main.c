#include <stdio.h>
#include "lexer.h"
#include <string.h>

int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Invalid number of Arguments:\n");
        return 1;
    }
    initializeLexer(argv[1]);
    check_error();
    printf("Opened file: %s\n", argv[1]);

    printf("%-20s %-20s\n", "TOKEN", "TYPE");
    char ptype[30];
    Token token;
    while ((token = getNextToken()).type != END_OF_FILE)
    {

        if (token.type == 0)
        {
            strcpy(ptype, "RESERVED_KEYWORD");
        }
        else if (token.type == 1)
        {
            strcpy(ptype, "NON_RESERVED_KEYWORD");
        }
        else if (token.type == 2)
        {
            strcpy(ptype, "OPERATOR");
        }
        else if (token.type == 3)
        {
            strcpy(ptype, "SPECIAL_CHARACTER");
        }
        else if (token.type == 4)
        {
            strcpy(ptype, "CONSTANT");
        }
        else if (token.type == 5)
        {
            strcpy(ptype, "IDENTIFIER");
        }
        else if (token.type == 6)
        {
            strcpy(ptype, "STRING_LITERAL");
        }
        else if (token.type == 7)
        {
            strcpy(ptype, "END_OF_FILE");
        }
        else if (token.type == 8)
        {
            strcpy(ptype, "HEADER_FILE");
        }
        else if (token.type == 9)
        {
            strcpy(ptype, "PREPROCESSOR_DIRECTIVE");
        }
        else
        {
            strcpy(ptype, "UNKNOWN");
        }
        printf("%-20s %-20s\n", token.lexeme, ptype);
    }

    return 0;
}
