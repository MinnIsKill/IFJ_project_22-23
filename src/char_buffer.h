#ifndef CHAR_BUFFER_INCLUDED
#define CHAR_BUFFER_INCLUDED

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "dbg.h"

typedef struct char_buffer
{
    size_t size;
    size_t len;
    char* buffer;
} char_buffer;

bool char_buffer_init(char_buffer* buf);
bool char_buffer_add(char_buffer* buf, int c);
size_t char_buffer_len(char_buffer* buf);
char* char_buffer_cstr(char_buffer* buf);
int char_buffer_get(char_buffer* buf, size_t index);
bool char_buffer_equals(char_buffer* buf, const char* str);
void char_buffer_clear(char_buffer* buf);
bool char_buffer_resize(char_buffer* buf, size_t size);
void char_buffer_destroy(char_buffer* buf);

#endif
