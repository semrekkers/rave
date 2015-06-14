#ifndef RAVE_TOKEN_H_
#define RAVE_TOKEN_H_

#include "rave.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ILLEGAL = 0,
    END,
    COMMENT,

    // literals
    IDENTIFIER,     // abc
    INT,            // 123
    FLOAT,          // 1.23
    CHAR,           // 'a'
    STRING,         // "abc"

    // keywords
    FUNC,           // func
    RETURN,         // return

    // operators
    ADD,            // +
	SUB,            // -
	MUL,            // *
	DIV,            // /
	REM,            // %
} rv_tokentype;

typedef struct {
    char type;
    char *value;
    uint line;
} rv_token;

#ifdef __cplusplus
}
#endif

#endif // RAVE_TOKEN_H_
