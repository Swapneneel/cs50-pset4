// Resizing the the bmp with float scale

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Useage: resize scale infile outfile.\n");
        return 1;
    }
    else
    {
        // convert to float
        float scale = atof(argv[1]);
    }
    return 0;
}