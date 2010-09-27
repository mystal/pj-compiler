#include "exprprods.h"

const production productions[41] =
{
    {0,1}, {1,3}, {1,1}, {2,3}, {2,1}, {3,3}, {3,1}, {4,1}, {4,1}, {4,1},
    {4,1}, {4,1}, {4,2}, {4,2}, {4,2}, {4,3}, {5,4}, {11,4}, {6,1}, {6,1},
    {6,1}, {6,1}, {6,1}, {6,1}, {7,1}, {7,1}, {7,1}, {8,1}, {8,1}, {8,1},
    {8,1}, {8,1}, {9,1}, {9,1}, {9,1}, {9,1}, {9,1}, {9,1}, {10,1}, {10,1},
    {12,1}
};

const char *prodStrings[41] = 
{
   "start --> expr",
   "expr --> expr relop primary",
   "expr --> primary",
   "primary --> primary addop term",
   "primary --> term",
   "term --> term multop factor",
   "term --> factor",
   "factor --> array",
   "factor --> builtin",
   "factor --> constant",
   "factor --> id",
   "factor --> fileptr",
   "factor --> not factor",
   "factor --> - factor",
   "factor --> + factor",
   "factor --> ( expr )",
   "builtin --> id ( expr )",
   "array --> id [ expr ]",
   "relop --> <",
   "relop --> <=",
   "relop --> =",
   "relop --> >=",
   "relop --> >",
   "relop --> <>",
   "addop --> +",
   "addop --> -",
   "addop --> or",
   "multop --> *",
   "multop --> /",
   "multop --> div",
   "multop --> mod",
   "multop --> and",
   "constant --> realcon",
   "constant --> intgrcon",
   "constant --> boolcon",
   "constant --> alfacon",
   "constant --> charcon",
   "constant --> strngcon",
   "boolcon --> true",
   "boolcon --> false",
   "s' --> start $"
};

const char *prodString(unsigned int i)
{
    return prodStrings[i];
}
