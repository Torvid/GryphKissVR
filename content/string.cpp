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

void CatStrings(int SourceACount, const char* SourceA,
    int SourceBCount, const char* SourceB,
    int DestCount, char* Dest)
{
    char* DestCounter = Dest;
    for (int i = 0; i < SourceACount; i++)
    {
        *DestCounter++ = *SourceA++;
    }

    for (int i = 0; i < SourceBCount; i++)
    {
        *DestCounter++ = *SourceB++;
    }

    *DestCounter++ = 0;
}
void CatStrings3(int SourceACount, const char* SourceA,
    int SourceBCount, const char* SourceB,
    int SourceCCount, const char* SourceC,
    int DestCount, char* Dest)
{
    char* DestCounter = Dest;
    for (int i = 0; i < SourceACount; i++)
    {
        *DestCounter++ = *SourceA++;
    }

    for (int i = 0; i < SourceBCount; i++)
    {
        *DestCounter++ = *SourceB++;
    }

    for (int i = 0; i < SourceCCount; i++)
    {
        *DestCounter++ = *SourceC++;
    }

    *DestCounter++ = 0;
}
char* StringAppend(char* str, const char* value)
{
    int current_length = StringLength(str);
    int newstring_length = StringLength(value);
    for (int i = 0; i < newstring_length; i++)
    {
        //if (current_length + i >= max)
        //    break;
        str[current_length + i] = value[i];
    }
    return str;
}
void StringAppend(char* str, char* str1, const char* str2)
{
    StringAppend(str, str1);
    StringAppend(str, str2);
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
        StringAppend(result, "true");
    }
    else
    {
        StringAppend(result, "false");
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

    CatStrings3(digits, intigerPartString, 
        1, ".", 
        precision, decimalPartString, 
        digits + 1 + precision, result);
}

char* StringAppend(char* str, int value)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    StringAppend(str, convertedString);
    return str;
}
void StringAppend(char* str, char* str2, int value)
{
    StringAppend(str, str2);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, char* str3, int value)
{
    StringAppend(str, str2);
    StringAppend(str, str3);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, int value, char* str3)
{
    StringAppend(str, str2);
    StringAppend(str, value);
    StringAppend(str, str3);
}

char* StringAppend(char* str, bool value)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    StringAppend(str, convertedString);
    return str;
}
void StringAppend(char* str, char* str2, bool value)
{
    StringAppend(str, str2);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, bool value, char* str3)
{
    StringAppend(str, str2);
    StringAppend(str, value);
    StringAppend(str, str3);
}

char* StringAppend(char* str, float value)
{
    char convertedString[100] = {};
    ToString(convertedString, value);
    StringAppend(str, convertedString);
    return str;
}
void StringAppend(char* str, float value, char* str2)
{
    StringAppend(str, value);
    StringAppend(str, str2);
}
void StringAppend(char* str, char* str2, float value)
{
    StringAppend(str, str2);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, float value, char* str3)
{
    StringAppend(str, str2);
    StringAppend(str, value);
    StringAppend(str, str3);
}

char* StringAppend(char* str, float3 value)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    StringAppend(str, "(");
    ToString(convertedString, value.x);
    StringAppend(str, convertedString);
    StringAppend(str, ", ");
    ToString(convertedString, value.y);
    StringAppend(str, convertedString);
    StringAppend(str, ", ");
    ToString(convertedString, value.z);
    StringAppend(str, convertedString);
    StringAppend(str, ")");
    return str;
}
void StringAppend(char* str, char* str2, float3 value)
{
    StringAppend(str, str2);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, float3 value, char* str3)
{
    StringAppend(str, str2);
    StringAppend(str, value);
    StringAppend(str, str3);
}

char* StringAppend(char* str, float2 value)
{
    char convertedString[100] = {};
    ToString(convertedString, value.x);
    StringAppend(str, "(");
    ToString(convertedString, value.x);
    StringAppend(str, convertedString);
    StringAppend(str, ", ");
    ToString(convertedString, value.y);
    StringAppend(str, convertedString);
    StringAppend(str, ")");
    return str;
}
void StringAppend(char* str, char* str2, float2 value)
{
    StringAppend(str, str2);
    StringAppend(str, value);
}
void StringAppend(char* str, char* str2, float2 value, char* str3)
{
    StringAppend(str, str2);
    StringAppend(str, value);
    StringAppend(str, str3);
}
// Takes the bit-pattern of a float and smashes it to an int.
char* CoerceStringAppend(char* str, float3 value)
{
    if (value.x == 0)
        value.x = 0;
    if (value.y == 0)
        value.y = 0;
    if (value.z == 0)
        value.z = 0;
    char convertedString[100] = {};
    StringAppend(str, "float3_coerce(");
    StringAppend(str, *(int*)&value.x);
    StringAppend(str, ", ");
    StringAppend(str, *(int*)&value.y);
    StringAppend(str, ", ");
    StringAppend(str, *(int*)&value.z);
    StringAppend(str, ")");
    return str;
}
// Takes the bit-pattern of a float and smashes it to an int.
char* CoerceStringAppend(char* str, float value)
{
    char convertedString[100] = {};
    StringAppend(str, "float_coerce(");
    StringAppend(str, *(int*)&value);
    StringAppend(str, ")");
    return str;
}
