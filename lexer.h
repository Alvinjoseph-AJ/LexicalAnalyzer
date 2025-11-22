#ifndef LEXER_H
#define LEXER_H
#define MAX_KEYWORDS 20
#define MAX_RES_KEYWORDS 19
#define MAX_NO_KEYWORDS 13
#define MAX_TOKEN_SIZE 100

#define RED        "\033[1;31m"
#define GREEN      "\033[1;32m"
#define YELLOW     "\033[1;33m"
#define BLUE       "\033[1;34m"
#define RESET      "\033[1;37m"

typedef enum {
    RESERVED_KEYWORD,
    NON_RESERVED_KEYWORD,
    OPERATOR,
    SPECIAL_CHARACTER,
    CONSTANT,
    IDENTIFIER,
    STRING_LITERAL,
    END_OF_FILE,
    HEADER_FILE,
    PREPROCESSOR_DIRECTIVE,
    UNKNOWN
} TokenType;

typedef struct {
    char lexeme[MAX_TOKEN_SIZE];
    TokenType type;
    int line;
}Token;

void initializeLexer(const char* filename);
Token getNextToken();
void categorizeToken(Token* token);
int isresKeyword(const char* str);
int isnonresKeyword(const char* str);
int isOperator(const char* str);
int isSpecialCharacter(char ch);
int isConstant(const char* str);
int isIdentifier(const char* str);
void check_error();
void check_error_count(int last_line,int curly_line,int square_line,int parenthesis_line,int double_quote_line,int single_quote_line);
void check_semicolon();
#endif
