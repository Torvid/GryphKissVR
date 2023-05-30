#pragma once

#include "math.cpp"

int StringLength(const char* checkString)
{
    int i = 0;
    while (checkString[i] != 0)
    {
        i++;
    }
    return i;
}
int PrintableLength(const char* checkString)
{
    int i = 0;
    while (checkString[i] != 0 && checkString[i] != '\n' && checkString[i] != '\r')
    {
        i++;
    }
    return i;
}
int DistanceToNewline(const char* checkString)
{
    int i = 0;
    while (true)
    {
        if (checkString[i] == 0 || checkString[i] == '\n')
            break; 
        i++;
    }
    return i;
}

int StringCountOccurances(char* string, char searchFor)
{
    int result = 0;
    for (int i = 0; i < StringLength(string); i++)
    {
        if (string[i] == searchFor)
        {
            result++;
        }
    }
    return result;
}

char* StringCopy(char* target, const char* string)
{
    int i = 0;
    for (i = 0; i < StringLength(string); i++)
    {
        target[i] = string[i];
    }
    return target + i;
}
char* StringCopyLength(char* target, const char* string, int length)
{
    int i = 0;
    for (i = 0; i < min(length, StringLength(string)); i++)
    {
        target[i] = string[i];
    }
    return target + i;
}
void StringClear(char* target)
{
    int len = StringLength(target);
    for (int i = 0; i < len; i++)
    {
        target[i] = 0;
    }
}

bool EndsWith(char* inputString, const char* checkString)
{
    int endOfString = StringLength(inputString);
    int checkStringSize = StringLength(checkString);

    // Loop through string to check if the ending matches
    for (int i = 0; i < checkStringSize; i++)
    {
        if (checkString[i] != inputString[(endOfString - checkStringSize) + i])
        {
            return false;
        }
    }
    return true;
}

bool StartsWith(char* inputString, const char* checkString)
{
    int endOfString = StringLength(inputString);
    int checkStringSize = StringLength(checkString);

    // Loop through string to check if the ending matches
    for (int i = 0; i < checkStringSize; i++)
    {
        if (checkString[i] != inputString[i])
        {
            return false;
        }
    }
    return true;
}

bool StartsWithIngoreWhitespace(char* inputString, const char* checkString)
{
    int endOfString = StringLength(inputString);
    int checkStringSize = StringLength(checkString);

    while (inputString[0] == ' ' || inputString[0] == '\t' || inputString[0] == '\r' || inputString[0] == '\n')
    {
        inputString++;
    }

    // Loop through string to check if the ending matches
    for (int i = 0; i < checkStringSize; i++)
    {
        if (checkString[i] != inputString[i])
        {
            return false;
        }
    }
    return true;
}

int IndexOf(char* inputString, char search)
{
    int len = StringLength(inputString);

    for (int i = 0; i < len; i++)
    {
        if (inputString[i] == search)
            return i;
    }
    return -1;
}

int LastIndexOf(char* inputString, char search)
{
    int len = StringLength(inputString);
    int lastIndex = -1;
    for (int i = 0; i < len; i++)
    {
        if (inputString[i] == search)
            lastIndex = i;
    }
    return lastIndex;
}

bool StringEquals(char* stringA, char* stringB)
{
    int stringALength = StringLength(stringA);
    int stringBLength = StringLength(stringB);

    if (stringALength != stringBLength)
        return false;

    for (int i = 0; i < stringALength; i++)
    {
        if (stringA[i] != stringB[i])
            return false;
    }

    return true;
}

char* _StringAppend(char* str, const char* value)
{
    int current_length = StringLength(str);
    int newstring_length = StringLength(value);
    for (int i = 0; i < newstring_length; i++)
    {
        str[current_length + i] = value[i];
    }
    return str;
}
char* StringAppendMax(char* str, const char* value, int max)
{
    int current_length = StringLength(str);
    int newstring_length = StringLength(value);
    for (int i = 0; i < newstring_length; i++)
    {
        if (current_length + i >= max)
            break;
        str[current_length + i] = value[i];
    }
    return str;
}

int iabs(int a)
{
    if (a < 0)
        return -a;
    return a;
}
void ToString(char* result, int a)
{
    if (a == 0 || ((*(uint32*)&a) == 2147483648))
    {
        result[0] = '0';
        result[1] = 0;
        return;
    }

    int num = a;
    int digits = -1;
    do
    {
        digits++;
        num /= 10;
    } while (num != 0);


    int offset = 0;
    if (a < 0)
    {
        result[0] = '-';
        offset = 1;
    }

    for (int i = 0; i < (digits + 1); i++)
    {
        int current_number = 1;
        // 0, 10, 100, 1000
        for (int j = 0; j < i; j++)
        {
            current_number *= 10;
        }
        int d = ((int)iabs(a) / current_number) % 10;
        result[(digits - i) + offset] = d + 48;
    }
    if (a < 0)
    {
        result[(digits + 2)] = 0;
    }
    else
    {
        result[(digits + 1)] = 0;
    }
}
void ToString(char* result, bool value)
{
    if (value != 0)
    {
        _StringAppend(result, "true");
    }
    else
    {
        _StringAppend(result, "false");
    }
}
int pow10(int count)
{
    int current_number = 1;
    // 0, 10, 100, 1000
    for (int j = 0; j < count; j++)
    {
        current_number *= 10;
    }
    return current_number;
}
int digitCount(int a)
{
    int num = a;
    int digits = -1;
    do
    {
        digits++;
        num /= 10;
    } while (num != 0);
    return digits;
}
void ToString(char* result, float a, int precision = 4)
{
    if (a == 0)
    {
        result[0] = '0';
        result[1] = 0;
        return;
    }

    int extraDigits = 1;
    int digits = digitCount(abs(a)) + 1;
    int intigerPart = floor(a);
    if (a < 0)
    {
        intigerPart = ceil(a);
        digits += 1;
        //precision -= 1;
    }

    float q = (abs(a) - abs(intigerPart)) * pow10(precision);
    int decimalPart = abs(q);

    char intigerPartString[100] = {};
    char decimalPartString[100] = {};
    if (a < 0 && intigerPart == 0)
    {
        ToString(intigerPartString+1, intigerPart);
        intigerPartString[0] = '-';
    }
    else
        ToString(intigerPartString, intigerPart);
    ToString(decimalPartString, decimalPart);

    _StringAppend(result, intigerPartString);
    _StringAppend(result, ".");
    _StringAppend(result, decimalPartString);
}

//char* _StringAppend(char* str, bool value)
//{
//    char convertedString[100] = {};
//    ToString(convertedString, value);
//    _StringAppend(str, convertedString);
//    return str;
//}
char* _StringAppend(char* str, int value)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    _StringAppend(str, convertedString);
    return str;
}
void _StringAppend(char* str, float value)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    _StringAppend(str, convertedString);
}
char* _StringAppend(char* str, float2 value)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    _StringAppend(str, "(");
    ToString(convertedString, value.x);
    _StringAppend(str, convertedString);
    _StringAppend(str, ", ");
    ToString(convertedString, value.y);
    _StringAppend(str, convertedString);
    _StringAppend(str, ")");
    return str;
}
char* _StringAppend(char* str, float3 value)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    _StringAppend(str, "(");
    ToString(convertedString, value.x);
    _StringAppend(str, convertedString);
    _StringAppend(str, ", ");
    ToString(convertedString, value.y);
    _StringAppend(str, convertedString);
    _StringAppend(str, ", ");
    ToString(convertedString, value.z);
    _StringAppend(str, convertedString);
    _StringAppend(str, ")");
    return str;
}

#define GET_MACRO3(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, NAME, ...) NAME
#define StringAppend(...) GET_MACRO3(__VA_ARGS__, \
    StringAppend17, \
    StringAppend16, \
    StringAppend15, \
    StringAppend14, \
    StringAppend13, \
    StringAppend12, \
    StringAppend11, \
    StringAppend10, \
    StringAppend9, \
    StringAppend8, \
    StringAppend7, \
    StringAppend6, \
    StringAppend5, \
    StringAppend4, \
    StringAppend3, \
    StringAppend2, \
    StringAppend1, \
    "dummy")(__VA_ARGS__)

#define StringAppend2(str, value1) _StringAppend(str, value1); 
#define StringAppend3(str, value1, value2) _StringAppend(str, value1); StringAppend2(str, value2);
#define StringAppend4(str, value1, ...)  _StringAppend(str, value1); StringAppend3(str, __VA_ARGS__);
#define StringAppend5(str, value1, ...)  _StringAppend(str, value1); StringAppend4(str, __VA_ARGS__);
#define StringAppend6(str, value1, ...)  _StringAppend(str, value1); StringAppend5(str, __VA_ARGS__);
#define StringAppend7(str, value1, ...)  _StringAppend(str, value1); StringAppend6(str, __VA_ARGS__);
#define StringAppend8(str, value1, ...)  _StringAppend(str, value1); StringAppend7(str, __VA_ARGS__);
#define StringAppend9(str, value1, ...)  _StringAppend(str, value1); StringAppend8(str, __VA_ARGS__);
#define StringAppend10(str, value1, ...) _StringAppend(str, value1); StringAppend9(str, __VA_ARGS__);
#define StringAppend11(str, value1, ...) _StringAppend(str, value1); StringAppend10(str, __VA_ARGS__);
#define StringAppend12(str, value1, ...) _StringAppend(str, value1); StringAppend11(str, __VA_ARGS__);
#define StringAppend13(str, value1, ...) _StringAppend(str, value1); StringAppend12(str, __VA_ARGS__);
#define StringAppend14(str, value1, ...) _StringAppend(str, value1); StringAppend13(str, __VA_ARGS__);
#define StringAppend15(str, value1, ...) _StringAppend(str, value1); StringAppend14(str, __VA_ARGS__);
#define StringAppend16(str, value1, ...) _StringAppend(str, value1); StringAppend15(str, __VA_ARGS__);
#define StringAppend17(str, value1, ...) _StringAppend(str, value1); StringAppend16(str, __VA_ARGS__);
