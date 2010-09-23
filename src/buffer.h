#pragma once

#include <stdio.h>

/**
 * Gets the next character from the buffer. Fills the buffer
 * as necessary.
 **/
char bufferGetChar();

/**
 * Pushes back a number of characters onto the buffer.
 **/
void bufferPushBack(unsigned int);

/**
 * Retrieves the current line number.
 **/
unsigned int bufferLineNumber();

/**
 * Gets the current position in the buffer.
 **/
unsigned int bufferPos();

/**
 * Print out the current line in the buffer.
 **/
void bufferPrint(FILE *);
