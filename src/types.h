#ifndef TYPES_H
#define TYPES_H


#include <stdint.h>


typedef char     BYTE;
typedef uint16_t WORD;
typedef short    SHORT;
typedef uint32_t DWORD;

typedef struct {
    WORD length;
    BYTE characters[];
} STRING;


#endif // TYPES_H
