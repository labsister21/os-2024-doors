#include <stdint.h>
#include <stddef.h>
#include "header/stdlib/string.h"

void *memset(void *s, int c, size_t n)
{
    uint8_t *buf = (uint8_t *)s;
    for (size_t i = 0; i < n; i++)
        buf[i] = (uint8_t)c;
    return s;
}

void *memcpy(void *restrict dest, const void *restrict src, size_t n)
{
    uint8_t *dstbuf = (uint8_t *)dest;
    const uint8_t *srcbuf = (const uint8_t *)src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *buf1 = (const uint8_t *)s1;
    const uint8_t *buf2 = (const uint8_t *)s2;
    for (size_t i = 0; i < n; i++)
    {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n)
{
    uint8_t *dstbuf = (uint8_t *)dest;
    const uint8_t *srcbuf = (const uint8_t *)src;
    if (dstbuf < srcbuf)
    {
        for (size_t i = 0; i < n; i++)
            dstbuf[i] = srcbuf[i];
    }
    else
    {
        for (size_t i = n; i != 0; i--)
            dstbuf[i - 1] = srcbuf[i - 1];
    }

    return dest;
}

void strcat(char* dest, char* src){
    int i = 0;
    while(dest[i] != '\0'){
        i++;
    }
    int j = 0;
    while(src[j] != '\0'){
        dest[i] = src[j];
        i++;
        j++;
    }
    dest[i] = '\0';
}

void strset(char* str, char c, uint8_t len){
    int i = 0;
    while(i < len){
        str[i] = c;
        i++;
    }
    str[i] = '\0';
}

void strsplit(char* str, char delim, char result[16][256]) {
    int i = 0;
    int j = 0;
    int k = 0;
    while(str[i] != '\0'){
        if(str[i] == delim){
            result[j][k] = '\0';
            j++;
            k = 0;
        }else{
            result[j][k] = str[i];
            k++;
        }
        i++;
    }
    result[j][k] = '\0';
}

int strlen(char * str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        i++;
    }
    return i;
}

int contains(char * str, char c, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (str[i] == '\0')
        {
            return 0;
        }
        if (str[i] == c)
        {
            return 1;
        }
    }
    return 0;
}

void itoa(int32_t value, char *result)
{
    char *ptr = result, *ptr1 = result, tmp_char;
    int32_t tmp_value;

    do
    {
        tmp_value = value;
        value /= 10;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + (tmp_value - value * 10)];
    } while (value);

    // Apply negative sign
    if (tmp_value < 0)
        *ptr++ = '-';
    *ptr-- = '\0';
    while (ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr-- = *ptr1;
        *ptr1++ = tmp_char;
    }
}