#ifndef _STRING_H_
#define _STRING_H_

#pragma once

#include <types.h>

void*
memset(void *b, int c, size_t len);

int
memcmp(const void *b, const void *c, size_t len);

void*
memcpy(void *dest, const void *src, size_t n);

size_t
strlen(const char *str);

int
strcmp(const char *s1, const char *s2);

#endif