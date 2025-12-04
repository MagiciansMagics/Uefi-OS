#include <string.h>

void*
memset(void *b, int c, size_t len)
{
    int *b_char = (int *)b;

    if (b == NULL) return NULL;

    while(*b_char && len > 0)
    {
        *b_char = c;
        b_char++;
        len--;
    }

    return b;
}

int
memcmp(const void *b, const void *c, size_t len)
{
    const uint8_t *p = (const uint8_t *)b;
    const uint8_t *q = (const uint8_t *)c;

    while (len > 0)
    {
        if (*p != *q)
            return (*p - *q);
        len--;
        p++;
        q++;
    }

    return 0;
}

void*
memcpy(void *dest, const void *src, size_t n)
{ 
    uint8_t *csrc = (uint8_t*)src; 
    uint8_t *cdest = (uint8_t*)dest; 

    for (size_t i = 0; i < n; i++) 
        cdest[i] = csrc[i]; 

    return dest;
}

size_t
strlen(const char *str)
{
    size_t len = 0;

    if (str == NULL) return 0;

    while (str[len] != '\0')
        len++;

    return len;
}

int
strcmp(const char *s1, const char *s2)
{
    if (s1 == NULL && s2 == NULL) return 0;
    if (s1 == NULL) return -1;
    if (s2 == NULL) return 1;

    while (*s1 && (*s1 == *s2))
    {
        s1++;
        s2++;
    }

    return (unsigned char)*s1 - (unsigned char)*s2;
}