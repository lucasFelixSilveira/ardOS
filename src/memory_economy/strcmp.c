#include <stdint.h>

int8_t ardos_strcmp(const char s1[], const char s2[]) {
    while(*s1 && (*s1 == *s2)) s1++, s2++;
    return (int8_t)(*s1 - *s2);
}
