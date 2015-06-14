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
    uint pos;           // current position
    uint line;          // current line
    rv_vector tokens;   // tokens
    bool done;          // if done
} rv_lexer;

int rv_lexer_init(rv_lexer *lex, char *stream);
void rv_lexer_destroy(rv_lexer *lex);

void rv_lexer_one(rv_lexer *lex);
void rv_lexer_all(rv_lexer *lex);

#ifdef __cplusplus
}
#endif

#endif // RAVE_LEXER_H_
