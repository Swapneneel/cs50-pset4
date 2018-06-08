// This will recover the deleted jpegs from the memory card

#include <stdio.h>
#include <stdlib.h>

#include "header.h"

int main(int argc, char *argv[])
{
    // ensuring only one argument the card.raw can be entered
    if (argc != 2)
    {
        fprintf(stderr, "Useage: ./recover card.raw\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[1];

    // open input file
    FILE *inptr = fread(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Counld not open the %s.raw file.\n", infile);
        return 2;
    }

    // reading the infile for 512 bytes at a time
    int n = sizeof(inptr) / 512, track = 0;
    char* outfile_opened;

    for (int i = 0; i < n; i++)  // at the begining entire file is a sequence of blocks of size 512 bytes
    {
        JPEG bytes;

        fread(&bytes, sizeof(JPEG), 512, inptr);

        char *outfile;

        // condition for Jpeg's header file
        if (bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff && (bytes[3] & 0xf0) == 0xe0)
        {
            if (track != 0)  // excluding first jpeg
            {
                fclose(outfile_opened);
            }

            bool new_jpeg = TRUE;
            track++;  // new name generated
        }
        else
        {
            bool new_jpeg = FALSE;
        }

        if (new_jpeg)  // if a new one is found
        {
            // start writing to the new file just opned
            fwrite(&bytes, sizeof(JPEG), 512, img);
        }
        else
        {
            // continue writing to the previously opened file
            if (track != 0)
            fwrite(&bytes, sizeof(JPEG), 512, outfile_opened);
        }


    }  // head loop ends

    return 0;  // main on success
}