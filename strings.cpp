#include "strings.h"

int FLOAT_N = sizeof(float);
int LONG_N = sizeof(long);

//private function for float/char converion
unsigned char charToHexDigit(char c)
{
  if (c >= 'A')
    return c - 'A' + 10;
  else
    return c - '0';
}

//private function for float.char conversion
unsigned char stringToByte(char c[3])
{
  return charToHexDigit(c[0]) * 16 + charToHexDigit(c[1]);
}

//Converts a float val to hex string
void floatToChar(char* cOut, float fIn){
    FloatChar fc;
    char hex_buf[3];
    cOut[0] = '\0';
    fc.fVal = fIn;
    for (int i = 0; i < FLOAT_N; i++) {
        sprintf(hex_buf, "%02X", fc.cVal[i]);
        strcat(cOut, hex_buf);
    }
}

//Converts a long val to hex string
void longToChar(char* cOut, unsigned long lIn){
    LongChar lc;
    char hex_buf[3];
    cOut[0] = '\0';
    lc.lVal = lIn;
    for (int i = LONG_N - 1; i >= 0; i--) {
        sprintf(hex_buf, "%02X", lc.cVal[i]);
        strcat(cOut, hex_buf);
    }
}


//converts a hex string back to float
float charToFloat(char* cIn){
    FloatChar fc;
    char hex_buf[3];
    int c = 0;
    for (int i = 0; i < FLOAT_N; i++){
        substring(cIn, hex_buf, c, c + 1);
        c+=2;
        fc.cVal[i] = stringToByte(hex_buf);
    }
    return fc.fVal;
}

//pads strings with empty spaces, mainly for lcd display.
void padstring(char* str, int len) {
  int startIdx = strlen(str) - 1;
  for (int i = startIdx; i < len; i++) {
    str[i] = ' ';
  }
  ++str = '\0';
}

//gives a substring of a string, based on a start and end index.
void substring(char* inStr, char* outStr, int startIdx, int endIdx){
    for (int i = startIdx; i <= endIdx; i++) {
      outStr[i - startIdx] = inStr[i];
    }
    ++outStr = '\0';
}

//adds a char to a string.
void strcharcat(char* str, char value){
  str[strlen(str)] = value;
  ++str = '\0';
}
