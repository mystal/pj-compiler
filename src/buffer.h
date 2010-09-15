#pragma once

#define BUFFER_SIZE 102

char lineBuffer[BUFFER_SIZE];

char getChar();

void pushBack(unsigned int);

unsigned int getLineNumber();

unsigned int getBufferPos();
