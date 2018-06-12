// This will recover the deleted jpegs from the memory card

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Counld not open the %s.raw file.\n", infile);
        return 2;
    }  // upto ok

    JPEG *one_box = malloc(512);  // one box with size 512 bytes

    int found = 0, found_prev;

    char *outfile = NULL;  // file name string
    FILE *outptr = NULL;  // outfile pointer name

    while (fread(one_box, 512, 1, inptr) == 1)
    {
      if (*(one_box + 0) == 0xff && *(one_box + 1) == 0xd8 && *(one_box + 2) == 0xff &&
             (*(one_box + 3) & 0xf0) == 0xe0)
      {
          found++;

          // close the file previously opened
          if (found > 1)
          {
              fclose(outptr);
          }

          // file name & file opening for reading
          fprintf(outfile, "%03i.jpg", found);
          *outptr = fopen(outfile, "w");
          if (outptr == NULL)
          {
            // error message for not opening the outfile
            fprintf(stderr, "Could not create %s.\n", outfile);
            return 3;
          }
      }

      if (found > 0)  // wtite only when after found the 1st one
      {
          fwrite(one_box, 512, 1, outptr);
      }

      // free the alocated memory
      free(one_box);
    }


    return 0;  // main on success
}
