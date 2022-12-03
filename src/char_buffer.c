#include "char_buffer.h"

#define CHAR_BUFFER_INIT_SIZE       16

bool char_buffer_init(char_buffer* buf)
{
    buf->buffer = NULL;
    buf->len = 0;

    return char_buffer_resize(buf, CHAR_BUFFER_INIT_SIZE);
}

bool char_buffer_add(char_buffer* buf, int c)
{
    if (buf->len + 2 > buf->size)
    {
        if (!char_buffer_resize(buf, buf->size * 2))
            return false;
    }

    buf->buffer[buf->len++] = c;
    buf->buffer[buf->len] = '\0';
    return true;
}

void char_buffer_remove(char_buffer* buf, size_t n)
{
    if (buf->len < n)
        return;

    buf->len -= n;
}

size_t char_buffer_len(char_buffer* buf)
{
    return buf->len;
}

const char* char_buffer_cstr(char_buffer* buf)
{
    return buf->buffer;
}

int char_buffer_get(char_buffer* buf, size_t index)
{
    return buf->buffer[index];
}

const char* char_buffer_end(char_buffer* buf, size_t n)
{
    if (buf->len < n)
        return NULL;

    return buf->buffer + buf->len - n;
}

bool char_buffer_equals(char_buffer* buf, const char* other)
{
    if (buf->len != strlen(other))
        return false;

    return strncmp(buf->buffer, other, buf->len) == 0;
}

void char_buffer_clear(char_buffer* buf)
{
    buf->buffer[0] = '\0';
    buf->len = 0;
}

bool char_buffer_resize(char_buffer* buf, size_t size)
{
    infoprint("Reallocating for size %ld ...", size);
    char* buffer = realloc(buf->buffer, size);

    if (!buffer)
    {
        free(buf->buffer);

        dbgprint("Failed to allocate %ld bytes for char_buffer!", size);
        buf->buffer = NULL;
        buf->size = 0;

        return false;
    }

    buf->buffer = buffer;
    buf->size = size;

    return true;
}

void char_buffer_destroy(char_buffer* buf)
{
    if (buf->buffer)
        free(buf->buffer);

    buf->len = 0;
    buf->size = 0;
    buf->buffer = NULL;
}
