#include "mypow.h"

#include <stdio.h>

int mypow_int(int a, int exp)
{
    if (exp < 0)
    {
        fprintf(stderr, "exp must > 0.\n");
        return 0;
    }

    if (exp == 0)
    {
        return 1;
    }
    else
    {
        return a * mypow_int(a, exp - 1);
    }
    

}