#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lexer.h"

static FILE *fp;
static int currentchar;
// static int line_number = 1;  // Global line number tracker

static const char *res_keywords[MAX_RES_KEYWORDS] = {"const", "volatile", "extern", "auto", "register", "static", "signed", "unsigned", "short", "long", "double", "char", "int", "float", "struct", "union", "enum", "void", "typedef"};
static const char *non_res_keywords[MAX_KEYWORDS] = {"goto", "return", "continue", "break", "if", "else", "for", "while", "do", "switch", "case", "default", "sizeof"};
static const char *operators = "+-*/%=!<>|&";
static const char *specialCharacters = ",;{}()[]";

static int curly_count = 0, square_count = 0, parenthesis_count = 0, double_quotes = 0, single_quotes = 0;
static int error_flag = 0;

void initializeLexer(const char *filename)
{
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        perror("ERROR");
        exit(1);
    }

    currentchar = fgetc(fp);
    // line_number = 1;
}

Token getNextToken()
{
    Token token;
    token.type = UNKNOWN;
    int index = 0;
    token.lexeme[0] = '\0';
    // token.line = line_number;

    while (isspace(currentchar))
    {
        // printf("Current char: '%c'\n", currentchar);
        currentchar = fgetc(fp);
    }

    // Handle string literals
    if (currentchar == '"')
    {
        int i = 0;
        token.lexeme[i++] = currentchar; // opening quote
        currentchar = fgetc(fp);

        while (currentchar != '"' && currentchar != EOF)
        {
            // handle escape sequences like \" inside strings
            if (currentchar == '\\')
            {
                token.lexeme[i++] = currentchar;
                currentchar = fgetc(fp);
            }

            token.lexeme[i++] = currentchar;
            currentchar = fgetc(fp);

            if (i >= MAX_TOKEN_SIZE - 2)
                break; // prevent overflow
        }

        // closing quote
        if (currentchar == '"')
        {
            token.lexeme[i++] = '"';
            currentchar = fgetc(fp);
        }

        token.lexeme[i] = '\0';
        token.type = STRING_LITERAL; // or STRING_LITERAL if you add a new type
        return token;
    }

    if (currentchar == '#')
    {
        int flag = 0;
        char buffer[MAX_TOKEN_SIZE];
        int buff = 0;

        // Capture everything until newline
        while (currentchar != '\n' && currentchar != EOF)
        {
            if (buff < MAX_TOKEN_SIZE - 1)
                buffer[buff++] = currentchar;
            currentchar = fgetc(fp);
        }
        buffer[buff] = '\0';

        // Skip newline if present
        if (currentchar == '\n')
        {
            currentchar = fgetc(fp);
            // line_number++;
        }

        // Trim trailing spaces
        int len = strlen(buffer);
        while (len > 0 && isspace((unsigned char)buffer[len - 1]))
            buffer[--len] = '\0';

        strcpy(token.lexeme, buffer);
        int k = 0;
        while (buffer[k])
        {
            if (buffer[k] == '"')
            {
                flag++;
            }
            k++;
        }

        // ✅ Decide token type
        if (strstr(buffer, "#include") == buffer && (strchr(buffer, '<') || flag == 2)) // starts with #include
            token.type = HEADER_FILE;
        else
            token.type = PREPROCESSOR_DIRECTIVE;

        return token;
    }

    if (currentchar == EOF)
    {
        token.lexeme[0] = '\0';
        token.type = END_OF_FILE;
        return token;
    }

    if (isalpha(currentchar) || currentchar == '_')
    {
        int i = 0;
        while (isalnum(currentchar) || currentchar == '_')
        {
            token.lexeme[i++] = currentchar;
            currentchar = fgetc(fp);
        }
        token.lexeme[i] = '\0';
        categorizeToken(&token);
        return token;
    }

    // Number constant
    if (isdigit(currentchar))
    {
        int i = 0;
        while (isdigit(currentchar))
        {
            token.lexeme[i++] = currentchar;
            currentchar = fgetc(fp);
        }
        token.lexeme[i] = '\0';
        categorizeToken(&token);
        return token;
    }

    // Operator or special character (single-char only)
    token.lexeme[0] = currentchar;
    token.lexeme[1] = '\0';
    currentchar = fgetc(fp);
    categorizeToken(&token);

    return token;
}

void categorizeToken(Token *token)
{
    if (isresKeyword(token->lexeme))
    {
        token->type = RESERVED_KEYWORD;
    }
    else if (isnonresKeyword(token->lexeme))
    {
        token->type = NON_RESERVED_KEYWORD;
    }
    else if (isOperator(token->lexeme))
    {
        token->type = OPERATOR;
    }
    else if (isSpecialCharacter(token->lexeme[0]) && token->lexeme[1] == '\0')
    {
        token->type = SPECIAL_CHARACTER;
    }
    else if (isConstant(token->lexeme))
    {
        token->type = CONSTANT;
    }
    else if (isIdentifier(token->lexeme))
    {
        token->type = IDENTIFIER;
    }
    else
    {
        token->type = UNKNOWN;
    }
}

int isresKeyword(const char *str)
{
    for (int i = 0; i < MAX_RES_KEYWORDS; i++)
    {
        if (strcmp(res_keywords[i], str) == 0)
        {
            return 1;
        }
    }
    return 0;
}
int isnonresKeyword(const char *str)
{
    for (int i = 0; i < MAX_NO_KEYWORDS; i++)
    {
        if (strcmp(non_res_keywords[i], str) == 0)
        {
            return 1;
        }
    }
    return 0;
}

int isOperator(const char *str)
{
    if (strlen(str) == 1)
    {
        if (strchr(operators, str[0]))
        {
            return 1;
        }
    }
    return 0;
}

int isSpecialCharacter(char ch)
{
    return strchr(specialCharacters, ch) != NULL;
}

int isConstant(const char *str)
{
    int i = 0;
    while (str[i])
    {
        if (!isdigit(str[i]))
        {
            return 0;
        }
        i++;
    }
    return 1;
}

int isIdentifier(const char *str)
{
    if (!isalpha(str[0]) && str[0] != '_')
    {
        return 0;
    }
    for (int i = 0; str[i]; i++)
    {
        if (!isalnum(str[i]) && str[i] != '_')
        {
            return 0;
        }
    }
    return 1;
}

void check_error()
{
    char ch, prev_ch = 0;
    int double_quotes_flag = 0, single_quotes_flag = 0;
    int local_line_number = 1;

    curly_count = square_count = parenthesis_count = 0;
    error_flag = 0;
    double_quotes = single_quotes = 0;

    int last_curly_line = 0, last_square_line = 0, last_parenthesis_line = 0;
    int last_double_quote_line = 0, last_single_quote_line = 0;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n')
            local_line_number++;

        /* DOUBLE QUOTES — handle escape sequences */
        if (ch == '"' && !double_quotes_flag && prev_ch != '\\')
        {
            double_quotes++;
            double_quotes_flag = 1;
            last_double_quote_line = local_line_number;
        }
        else if (ch == '"' && double_quotes_flag && prev_ch != '\\')
        {
            double_quotes--;
            double_quotes_flag = 0;
        }

        /* SINGLE QUOTES — handle escape sequences and count correctly */
        if (ch == '\'' && !single_quotes_flag && prev_ch != '\\')
        {
            single_quotes++;
            single_quotes_flag = 1;
            last_single_quote_line = local_line_number;

            char next = fgetc(fp);
            if (next == '\n') local_line_number++;
            if (next == EOF || next == '\n')
            {
                printf(RED "ERROR: Unterminated single quote at line %d\n" RESET, local_line_number);
                error_flag = 1;
                break;
            }

            if (next == '\'')
            {
                printf(RED "ERROR: Empty character constant at line %d\n" RESET, local_line_number);
                error_flag = 1;
                single_quotes_flag = 0;
                single_quotes--;
                prev_ch = next;
                continue;
            }

            if (next == '\\')
            {
                char esc = fgetc(fp);
                if (esc == '\n') local_line_number++;
                if (esc == EOF || esc == '\n')
                {
                    printf(RED "ERROR: Invalid escape sequence at line %d\n" RESET, local_line_number);
                    error_flag = 1;
                    break;
                }
                char closing = fgetc(fp);
                if (closing == '\n') local_line_number++;
                if (closing != '\'')
                {
                    printf(RED "ERROR: Too many characters in single quotes at line %d\n" RESET, local_line_number);
                    error_flag = 1;
                }
            }
            else
            {
                char closing = fgetc(fp);
                if (closing == '\n') local_line_number++;
                if (closing != '\'')
                {
                    printf(RED "ERROR: Too many characters in single quotes at line %d\n" RESET, local_line_number);
                    error_flag = 1;
                }
            }

            single_quotes--;
            single_quotes_flag = 0;
            prev_ch = ch;
            continue;
        }
        else if (ch == '\'' && single_quotes_flag && prev_ch != '\\')
        {
            single_quotes--;
            single_quotes_flag = 0;
        }

        /* SKIP BRACKETS if inside quotes */
        if (double_quotes_flag || single_quotes_flag)
        {
            prev_ch = ch;
            continue;
        }

        /* BRACKET COUNTING — handle mismatch on closing */
        if (ch == '{')
        {
            curly_count++;
            last_curly_line = local_line_number;
        }
        else if (ch == '}')
        {
            if (curly_count <= 0)
            {
                printf(RED "ERROR: Extra closing curly bracket at line %d\n" RESET, local_line_number);
                error_flag = 1;
            }
            else
                curly_count--;
        }
        else if (ch == '[')
        {
            square_count++;
            last_square_line = local_line_number;
        }
        else if (ch == ']')
        {
            if (square_count <= 0)
            {
                printf(RED "ERROR: Extra closing square bracket at line %d\n" RESET, local_line_number);
                error_flag = 1;
            }
            else
                square_count--;
        }
        else if (ch == '(')
        {
            parenthesis_count++;
            last_parenthesis_line = local_line_number;
        }
        else if (ch == ')')
        {
            if (parenthesis_count <= 0)
            {
                printf(RED "ERROR: Extra closing parenthesis at line %d\n" RESET, local_line_number);
                error_flag = 1;
            }
            else
                parenthesis_count--;
        }

        prev_ch = ch;
    }

    check_error_count(local_line_number,
                      last_curly_line,
                      last_square_line,
                      last_parenthesis_line,
                      last_double_quote_line,
                      last_single_quote_line);

    check_semicolon();

    rewind(fp);
    currentchar = fgetc(fp);
}


// void check_semicolon()
// {
//     rewind(fp);
//     char ch, prev_ch = 0;
//     int line_number = 1;
//     int double_quotes_flag = 0, single_quotes_flag = 0;
//     char line_buf[1024]; // larger buffer
//     int idx = 0;

//     int block_depth = 0;   // track braces for if/else blocks
//     int skip_enum = 0;     // flag to skip enum
//     int enum_brace_depth = 0;

//     while ((ch = fgetc(fp)) != EOF)
//     {
//         if (ch == '\n' || idx >= (int)(sizeof(line_buf) - 1))
//         {
//             line_buf[idx] = '\0';

//             // Trim leading spaces
//             char *trimmed = line_buf;
//             while (isspace((unsigned char)*trimmed))
//                 trimmed++;

//             // Skip empty lines and comments
//             if (*trimmed != '\0' && *trimmed != '#' && strncmp(trimmed, "//", 2) != 0)
//             {
//                 // Detect start of enum block
//                 if (!skip_enum && strstr(trimmed, "enum"))
//                     skip_enum = 1;

//                 // Skip checking inside enum
//                 if (skip_enum)
//                 {
//                     for (char *p = trimmed; *p; p++)
//                     {
//                         if (*p == '{') enum_brace_depth++;
//                         else if (*p == '}') enum_brace_depth--;
//                     }

//                     if (enum_brace_depth <= 0)
//                     {
//                         skip_enum = 0;
//                         enum_brace_depth = 0;
//                     }

//                     idx = 0;
//                     line_number++;
//                     continue;
//                 }

//                 // Skip only 'if' or 'else' lines themselves
//                 int skip_line = 0;
//                 if ((strncmp(trimmed, "else", 4) == 0 &&
//                      (isspace((unsigned char)trimmed[4]) || trimmed[4] == '{' || trimmed[4] == '\0')) ||
//                     (strncmp(trimmed, "if", 2) == 0 &&
//                      isspace((unsigned char)trimmed[2])))
//                 {
//                     skip_line = 1;
//                 }

//                 // Update block depth
//                 for (char *p = trimmed; *p; p++)
//                 {
//                     if (*p == '{') block_depth++;
//                     else if (*p == '}') block_depth--;
//                 }

//                 if (!skip_line)
//                 {
//                     // Trim trailing spaces
//                     char *end_ptr = trimmed + strlen(trimmed) - 1;
//                     while (end_ptr > trimmed && isspace((unsigned char)*end_ptr))
//                         end_ptr--;

//                     // Check missing semicolon
//                     if (*end_ptr != ';' &&
//                         *end_ptr != '{' &&
//                         *end_ptr != '}' &&
//                         !double_quotes_flag && !single_quotes_flag)
//                     {
//                         printf(RED "ERROR: Possible missing semicolon at line %d\n" RESET, line_number);
//                     }
//                 }
//             }

//             idx = 0;
//             line_number++;
//         }
//         else
//         {
//             line_buf[idx++] = ch;

//             // Track quotes
//             if (ch == '"' && prev_ch != '\\')
//                 double_quotes_flag = !double_quotes_flag;
//             if (ch == '\'' && prev_ch != '\\')
//                 single_quotes_flag = !single_quotes_flag;

//             prev_ch = ch;
//         }
//     }
// }

void check_semicolon()
{
    rewind(fp);
    char ch, prev_ch = 0;
    int line_number = 1;
    int double_quotes_flag = 0, single_quotes_flag = 0;
    char line_buf[1024];
    int idx = 0;

    int block_depth = 0;       // Track braces for blocks
    int skip_enum = 0;         // Flag to skip enum
    int enum_brace_depth = 0;

    while ((ch = fgetc(fp)) != EOF)
    {
        if (ch == '\n' || idx >= (int)(sizeof(line_buf) - 1))
        {
            line_buf[idx] = '\0';

            char *trimmed = line_buf;
            while (isspace((unsigned char)*trimmed)) trimmed++;

            if (*trimmed != '\0' && *trimmed != '#' && strncmp(trimmed, "//", 2) != 0)
            {
                // Skip enum block
                if (!skip_enum && strstr(trimmed, "enum")) skip_enum = 1;
                if (skip_enum)
                {
                    for (char *p = trimmed; *p; p++)
                    {
                        if (*p == '{') enum_brace_depth++;
                        else if (*p == '}') enum_brace_depth--;
                    }
                    if (enum_brace_depth <= 0) { skip_enum = 0; enum_brace_depth = 0; }
                    idx = 0; line_number++; continue;
                }

                // Count block braces
                for (char *p = trimmed; *p; p++)
                {
                    if (*p == '{') block_depth++;
                    else if (*p == '}') block_depth--;
                }

                // Extract first word for control check
                char first_word[64] = {0};
                sscanf(trimmed, "%63s", first_word);

                // Skip control statements and function definitions
                int skip_line = 0;
                if (strcmp(first_word, "if") == 0 ||
                    strcmp(first_word, "else") == 0 ||
                    strcmp(first_word, "while") == 0 ||
                    strcmp(first_word, "for") == 0 ||
                    strcmp(first_word, "switch") == 0 ||
                    strcmp(first_word, "do") == 0)
                {
                    skip_line = 1;
                }

                // Skip function definition lines: contains '(' and ')' before any '{'
                char *paren_open = strchr(trimmed, '(');
                char *paren_close = strchr(trimmed, ')');
                char *brace = strchr(trimmed, '{');
                if (paren_open && paren_close && (!brace || paren_close < brace))
                {
                    skip_line = 1;
                }

                if (!skip_line)
                {
                    // Trim trailing spaces
                    char *end_ptr = trimmed + strlen(trimmed) - 1;
                    while (end_ptr > trimmed && isspace((unsigned char)*end_ptr)) end_ptr--;

                    // Check missing semicolon
                    if (*end_ptr != ';' && *end_ptr != '{' && *end_ptr != '}' &&
                        !double_quotes_flag && !single_quotes_flag)
                    {
                        printf(RED "ERROR: Possible missing semicolon at line %d\n" RESET, line_number);
                    }
                }
            }

            idx = 0;
            line_number++;
        }
        else
        {
            line_buf[idx++] = ch;

            if (ch == '"' && prev_ch != '\\') double_quotes_flag = !double_quotes_flag;
            if (ch == '\'' && prev_ch != '\\') single_quotes_flag = !single_quotes_flag;
            prev_ch = ch;
        }
    }
}









void check_error_count(int last_line,int curly_line,int square_line,int parenthesis_line,int double_quote_line,int single_quote_line)
{
    if (curly_count != 0)
    {
        printf(RED "ERROR: INCORRECT NUMBER OF CURLY BRACKETS {} (around line %d)\n" RESET, curly_line);
        error_flag = 1;
    }
    if (square_count != 0)
    {
        printf(RED "ERROR: INCORRECT NUMBER OF SQUARE BRACKETS [] (around line %d)\n" RESET, square_line);
        error_flag = 1;
    }
    if (parenthesis_count != 0)
    {
        printf(RED "ERROR: INCORRECT NUMBER OF PARENTHESIS () (around line %d)\n" RESET, parenthesis_line);
        error_flag = 1;
    }
    if (double_quotes != 0)
    {
        printf(RED "ERROR: UNCLOSED DOUBLE QUOTES \"...\" (around line %d)\n" RESET, double_quote_line);
        error_flag = 1;
    }
    if (single_quotes != 0)
    {
        printf(RED "ERROR: UNCLOSED SINGLE QUOTES '...' (around line %d)\n" RESET, single_quote_line);
        error_flag = 1;
    }
}