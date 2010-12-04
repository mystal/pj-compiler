#pragma once

#include <stdbool.h>

typedef struct __string string;

/**
 * Creates and returns an empty string.
 **/
string *stringCreate(void);

/**
 * Appends the given character to the string.
 **/
void stringAppendChar(string *, char);

/**
 * Appends the given string to the string.
 **/
void stringAppendString(string *, string *);

/**
 * Appends the given character array of provided length to the string.
 **/
void stringAppendCharArray(string *, const char *, unsigned int);

/**
 * Removes a number of characteres from the end of the string.
 **/
void stringDrop(string *, unsigned int);

/**
 * Performs a case-insensitive comparison of the two strings, returning
 * an integer. If negative, the first string is smaller, if positive, the
 * first string is larger, and if zero they are equal.
 **/
int stringCompareString(string *, string *);

/**
 * Performs a case-insensitive comparison of the string and character array,
 * returning an integer. If negative, the first string is smaller, if
 * positive, the first string is larger, and if zero they are equal.
 **/
int stringCompareCharArray(string *, const char *, unsigned int);

/**
 * Gets the given string's length. 
 **/
unsigned int stringGetLength(string *);

/**
 * Gets a pointer to the given string's buffer.
 **/
const char *stringGetBuffer(string *);

/**
 * Returns an integer representation of the given string.
 **/
int stringToInt(string *);

/**
 * Returns a floating point representation of the given string.
 **/
float stringToFloat(string *);

/**
 * Returns a boolean representation of the given string.
 **/
bool stringToBool(string *);

/**
 * Deallocates memory for the string and sets its pointer to NULL.
 **/
void stringDestroy(string *);
