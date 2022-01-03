#define _DEFAULT_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#define HEADER_MEM(header) ((char *) (header + 1))
#define MEM_HEADER(memory) ((size_t *) memory - 1)

#define HEADER_SIZE(header) (*header & ~1)
#define HEADER_USED(header) (*header & 1)
#define HEADER_NEXT(header) (size_t *) (HEADER_MEM(header) + HEADER_SIZE(header))

char *heap_start;

void *malloc(size_t size)
{
    if (size == 0) return NULL;
    if (size & 1) size++;

    size_t *header = (size_t *) heap_start;
    bool found;

    if (header) {
        for (found = false; HEADER_SIZE(header); header = HEADER_NEXT(header)) {
            if (!HEADER_USED(header)) {
                if (HEADER_SIZE(header) < size) {
                    while (!HEADER_USED(HEADER_NEXT(header)) && HEADER_SIZE(HEADER_NEXT(header))) {
                        *header += HEADER_SIZE(HEADER_NEXT(header)) + sizeof(size_t);
                    }
                }

                if (HEADER_SIZE(header) >= size) {
                    found = true;
                    break;
                }
            }
        }
    }

    if (!found) {
        header = sbrk(size + sizeof(size_t));

        if (header == (void *) -1) {
            return NULL;
        }

        if (!heap_start) {
            heap_start = (char *) header;
        }

        *header = size;
    }

    *header = *header | 1;
    return HEADER_MEM(header);
}

void *calloc(size_t count, size_t size)
{
    void *memory = malloc(size * count);
    memset(memory, 0, size * count);
    return memory;
}

void *realloc(void *ptr, size_t size)
{
    if (ptr) {
        size_t *header = MEM_HEADER(ptr);

        if (HEADER_SIZE(header) >= size) {
            return ptr;
        } else {
            *header = HEADER_SIZE(header);
            void *memory = malloc(size);
            return memcpy(memory, ptr, HEADER_SIZE(header));
        }
    } else {
        return malloc(size);
    }
}

void free(void *ptr)
{
    if (ptr) {
        size_t *header = MEM_HEADER(ptr);
        *header = HEADER_SIZE(header);
    }
}

typedef struct {
    char *source;
    size_t length;
} Str;

void str_print(Str str)
{
    write(1, str.source, str.length);
    write(1, "\n", 1);
}

void str_append(Str *str, const char *cstr)
{
    const size_t len = strlen(cstr);
    str->source = realloc(str->source, str->length + len);
    memcpy(str->source + str->length, cstr, len);
    str->length += len;
}

void str_free(Str *str)
{
    free(str->source);
    str->length = 0;
}

int main(void)
{
    Str a = {0};
    str_append(&a, "foobar");

    Str b = {0};
    str_append(&b, "noice");

    str_print(a);
    str_print(b);

    str_append(&a, "yo lol");
    str_append(&b, "69 420");

    str_print(a);
    str_print(b);

    str_free(&a);
    str_free(&b);
    return 0;
}
