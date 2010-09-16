#pragma once

// Define size of buffer to hold 100 characters plus \n and \0
#define BUFFER_SIZE 102

/* Global buffer of current line in file */
char lineBuffer[BUFFER_SIZE];

/**
 * Gets the next character from the buffer. Fills the buffer
 * as necessary.
 **/
char getChar();

/**
 * Pushes back a number of characters onto the buffer.
 **/
void pushBack(unsigned int);

/**
 * Retrieves the current line number.
 **/
unsigned int getLineNumber();

/**
 * Gets the current position in the buffer.
 **/
unsigned int getBufferPos();
