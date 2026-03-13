#include "string.h"

// Function: Find a string length.
size_t strlen(const char* str){
    size_t i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}