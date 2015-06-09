#include "rave.h"

#include <string.h>
#include <stdarg.h>

void rv_log(const char *file, const uint line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[256];
    vsprintf(buffer, fmt, args);
    printf("[%s:%d] log: %s\n", file, line, buffer);
}

void rv_error(const char *file, const uint line, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char buffer[256];
    vsprintf(buffer, fmt, args);
    fprintf(stdout, "[%s:%d] error: %s\n", file, line, buffer);
    fflush(stdout);
    abort();
}

void rv_checkFailed(const char *file, const uint line, const char *cond) {
    fprintf(stdout, "[%s:%d] check failed: %s\n", file, line, cond);
    fflush(stdout);
    abort();
}

void* rv_malloc(uint size) {
    void *ptr = malloc(size);
    if (!ptr)
        RV_ERROR("out of memory");
    return ptr;
}

void* rv_realloc(void *old, uint newSize) {
    void *ptr = realloc(old, newSize);
    if (newSize && !ptr)
        RV_ERROR("out of memory");
    return ptr;
}

char* rv_strdup(char *str) {
    uint size = strlen(str);
    char *dup = (char*)malloc(++size);
    memcpy(dup, str, size);
    return dup;
}
