#ifndef RAVE_LEXER_H_
#define RAVE_LEXER_H_

#include "rave.h"
#include "token.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char *begin;        // begin of file
    char *cur;          // cursor
    uint line;          // current line
    uint length;        // stream length
    uint curPos;        // cursor position
    rv_vector tokens;   // tokens
    bool done;          // if done
} rv_lexer;

int rv_lexer_init(rv_lexer *lex, char *stream, uint streamLength);
void rv_lexer_destroy(rv_lexer *lex);

void rv_lexer_one(rv_lexer *lex);
void rv_lexer_all(rv_lexer *lex);

static inline bool rv_isLower(char c) {
    return ('a' <= c && c <= 'z');
}

static inline bool rv_isUpper(char c) {
    return ('A' <= c && c <= 'Z');
}

static inline bool rv_isAlpha(char c) {
    return rv_isLower(c) || rv_isUpper(c) || c == '_';
}

static inline bool rv_isNum(char c) {
    return ('0' <= c && c <= '9');
}

static inline bool rv_isAlphaNum(char c) {
    return rv_isAlpha(c) || rv_isNum(c);
}

static inline bool rv_isNewline(char c) {
    return (c == '\n');
}

static inline bool rv_isEOF(char c) {
    return (c == '\0');
}

static inline bool rv_isWhitespace(char c) {
    return (c == ' ') || (c == '\t') || rv_isNewline(c);
}

static inline bool rv_isSeparator(char c) {
    bool res = false;
    res |= (c == '{' || c == '}');
    res |= (c == '(' || c == ')');
    res |= (c == '[' || c == ']');
    res |= (c == ',' || c == ';');
    return res;
}

static inline bool rv_isOperator(char c) {
    bool res = false;
    res |= (c == '+' || c == '-');
    res |= (c == '*' || c == '/');
    res |= (c == '%' || c == '=');
    return res;
}

#ifdef __cplusplus
}
#endif

#endif // RAVE_LEXER_H_
