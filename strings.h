#ifndef String_H
#define String_H
#include <Arduino.h>

union FloatChar {
    float fVal;
    unsigned char cVal[sizeof(float)];
};

union LongChar {
    unsigned long lVal;
    unsigned char cVal[sizeof(long)];
};

void floatToChar(char*, float);
void longToChar(char*, unsigned long);
float charToFloat(char*);
void padstring(char*, int);
void substring(char*, char*, int, int);
void strcharcat(char*, char);

#endif
