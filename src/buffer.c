#include "buffer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdbool.h>

#include "directive.h"

unsigned int line = 1;
unsigned int bufferPos = 0;

/* Fills the buffer with the next line in the file from stdin. */
bool fillBuffer();

char getChar()
{
    // If reached end of line then refill buffer
    if (lineBuffer[bufferPos] == '\0')
        if (!fillBuffer())
            return '\0';
    return lineBuffer[bufferPos++];
}

void pushBack(unsigned int i)
{
    bufferPos -= i;
}

unsigned int getLineNumber()
{
    return line - 1;
}

unsigned int getBufferPos()
{
    return bufferPos - 1;
}

bool fillBuffer()
{
    if (fgets(lineBuffer, BUFFER_SIZE-1, stdin) != NULL)
    {
        for (int i = 0; lineBuffer[i] != '\0'; i++)
            // Validate input, change invalid characters to spaces
            if (!isprint(lineBuffer[i]) && lineBuffer[i] != '\n')
                lineBuffer[i] = ' ';
        if (directives[dir_listing])
            fprintf(stdout, "%d: %s", line++, lineBuffer);
        bufferPos = 0;
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
