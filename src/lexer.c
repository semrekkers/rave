#include "lexer.h"

int rv_lexer_init(rv_lexer *lex, char *stream, uint streamLength) {
    RV_CHECK(lex);
    RV_CHECK(stream);
    lex->begin = stream;
    lex->cur = stream;
    lex->line = 1;
    lex->length = streamLength;
    lex->curPos = 0;
    lex->done = false;
    return rv_vector_init(&lex->tokens, 32, sizeof(rv_token));
}

void rv_lexer_destroy(rv_lexer *lex) {
    rv_vector_destroy(&lex->tokens);
}

static bool strMatch(char *cmp, const char *str, const uint strLen) {
    for (uint i = 0; i < (strLen - 1); i++) {
        if (*cmp != *str)
            return false;
        cmp++; str++;
    }
    return true;
}

static char peek(rv_lexer *lex, int offset) {
    RV_CHECK(lex);
    ullong diff = lex->curPos + offset;
    if (diff < 0 || diff >= length)
        rv_panic(__FILE__, __LINE__, "illegal peek at %u with offset %d: %llu", lex->curPos, offset, diff);
    return *(lex->cur + offset);
}

static void consume(rv_lexer *lex) {
    RV_CHECK(lex);
    if (lex->curPos == lex->length)
        rv_panic(__FILE__, __LINE__, "end of file: cursor: %u, length: %u", lex->curPos, offset, diff);
    if (rv_isNewline(lex->cur))
        lex->line++;
    lex->cur++;
    lex->curPos++;
}

static void eat(rv_lexer *lex, uint amount) {
    // TODO: make this more cleaner
    for (uint i = 0; i < amount; i++) {
        consume(lex);
    }
}

static rv_vector newBuffer() {
    rv_vector buf;
    rv_vector_init(&buf, 8, sizeof(char));
    return buf;
}

static void addToken(rv_lexer *lex, rv_token *tok) {
    RV_CHECK(lex);
    RV_CHECK(tok);
    rv_vector_add(&lex->tokens, tok);
}

static void scanIdentifier(rv_lexer *lex) {
    rv_token tok;
    tok.type = rv_tokentype.IDENTIFIER;
    tok.line = lex->line;
    rv_vector buf = newBuffer();
    for (;;) {
        char c = peek(lex, 0);
        if (rv_isAlphaNum(c) {
            rv_vector_add(&buf, &c);
            consume(lex);
        }
        else {
            break;
        }
    }
    tok.value = buf.root;
    addToken(lex, &tok);
}

static void scanNumber(rv_lexer *lex) {
    rv_token tok;
    tok.type = rv_tokentype.INT;
    tok.line = lex->line;
    rv_vector buf = newBuffer();
    char c = peek(lex, 0);
    if (c == '+' || c == '-') {
        rv_vector_add(&buf, &c);
        consume(lex);
    }
    for (;;) {
        c = peek(lex, 0);
        if (rv_isNum(c)) {
            rv_vector_add(&buf, &c);
        }
        else if (c == '.') {
            tok.type = rv_tokentype.FLOAT;
            rv_vector_add(&buf, &c);
        }
        else {
            break;
        }
        consume(lex);
    }
    tok.value = buf.root;
    addToken(lex, &tok);
}

static void scanChar(rv_lexer *lex) {
    rv_token tok;
    tok.type = rv_tokentype.CHAR;
    tok.line = lex->line;
    rv_vector buf = newBuffer();
    consume(lex); // consume '
    bool esc = false;
    for (;;) {
        char c = peek(lex, 0);
        if (esc) {
            rv_vector_add(&buf, &c);
            esc = false;
        }
        else if (c == '\\') {
            rv_vector_add(&buf, &c);
            esc = true;
        }
        else if (c == '\'') {
            break;
        }
        else {
            if (buf.length > 8) {
                rv_panic(__FILE__, __LINE__, "unclosed char literal at line %u", tok.line);
            }
            rv_vector_add(&buf, &c);
        }
        consume(lex);
    }
    consume(lex); // consume other '
    tok.value = buf.root;
    addToken(lex, &tok);
}

static void scanString(rv_lexer *lex) {
    rv_token tok;
    tok.type = rv_tokentype.STRING;
    tok.line = lex->line;
    rv_vector buf = newBuffer();
    consume(lex); // consume "
    bool esc = false;
    for (;;) {
        char c = peek(lex, 0);
        if (esc) {
            rv_vector_add(&buf, &c);
            esc = false;
        }
        else if (c == '\\') {
            rv_vector_add(&buf, &c);
            esc = true;
        }
        else if (c == '"') {
            break;
        }
        else {
            if (c == '\n') {
                rv_panic(__FILE__, __LINE__, "unclosed string literal at line %u", tok.line);
            }
            rv_vector_add(&buf, &c);
        }
        consume(lex);
    }
    consume(lex); // consume other "
    tok.value = buf.root;
    addToken(lex, &tok);
}

static bool tryScanKeyword(rv_lexer *lex) {
    char *c = lex->cur;
    char keyword = 0;
    uint peeked = 0;

    switch (*c) {
        case 'f':
            if (strMatch(c, "func", sizeof("func"))) {
                keyword = rv_tokentype.FUNC;
                peeked = sizeof("func") - 1;
                goto scanKeyword;
            }
            return false;

        case 'r':
            if (strMatch(c, "return", sizeof("return"))) {
                keyword = rv_tokentype.RETURN;
                peeked = sizeof("return") - 1;
                goto scanKeyword;
            }
            return false;

        default:
            return false;
    }

    scanKeyword:
    rv_token tok;
    tok.type = keyword;
    tok.line = lex->line;
    tok.value = NULL;
    addToken(lex, &tok);
    eat(lex, peeked);
    return true;
}

// TODO: add support for delimiters and operators
