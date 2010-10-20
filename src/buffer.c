#include "buffer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#include "directive.h"

// Define size of buffer to hold 100 characters plus \n and \0
#define BUFFER_SIZE 102

/* Buffer of current line in file */
char buffer[BUFFER_SIZE] = {'\0'};

unsigned int line = 1;
unsigned int pos = 0;

/* Fills the buffer with the next line in the file from stdin. */
bool fillBuffer(void);

char bufferGetChar()
{
    // If reached end of line then refill buffer
    if (buffer[pos] == '\0')
        if (!fillBuffer())
            return '\0'; //End of file reached
    return buffer[pos++];
}

void bufferPushBack(unsigned int i)
{
    pos -= i;
}

unsigned int bufferLineNumber()
{
    return line - 1;
}

unsigned int bufferPos()
{
    return pos;
}

void bufferPrint(FILE *f)
{
    fprintf(f, "%s", buffer);
}

bool fillBuffer()
{
    if (fgets(buffer, BUFFER_SIZE-1, stdin) != NULL)
    {
        for (int i = 0; buffer[i] != '\0'; i++)
            // Validate input, change invalid characters to spaces
            if (!isprint(buffer[i]) && buffer[i] != '\n')
                buffer[i] = ' ';
        if (directives[dir_listing])
            fprintf(stdout, "%d: %s", line, buffer);
        line++;
        pos = 0;
        return true;
    }
    else
    {
        if (feof(stdin))
            fprintf(stderr, "End of file reached!\n");
        else if (ferror(stdin))
            fprintf(stderr, "Error reading file!\n");
        return false;
    }
}
