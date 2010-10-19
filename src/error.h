#pragma once

typedef enum __error
{
    err_unextoken,
    err_num
} error;

char *errorString(error);
