// This will recover the deleted jpegs from the memory card

#include <stdio.h>
#include <stdlib.h>


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

    // reading the infile for 512 bytes at a time
    int n = sizeof(inptr) / 512, track = 0;  // the box is
    char *card = malloc(sizeof(inptr));
    fread(card, 512, n, inptr);  // reading the data of card to the string

    char* outfile, outfile_opened;
    // the tracker for a new jpeg header
    int found = 0, found_prev = found;

    for (int i = 0; i < n; i++)  // at the begining entire file is a sequence of blocks of size 512 bytes
    {
        // read 512 byte box from the line of memory card data
        char *one_box = malloc(512);
        *one_box = *(card + i);
        bool new_found = FALSE;

        // condition for Jpeg's header file
        if (*(one_box + 0) == 0xff && *(one_box + 1) == 0xd8 && *(one_box + 2) == 0xff && (*(one_box + 3) & 0xf0) == 0xe0)
        {
          found++;
          new_found = TRUE;
        }
        if (new_found)
        {
          if (found > 1)  // it does mean that already first one is opened, and to be closed
          {
            fclose(outfile_opened);
          }
          sprintf(outfile, "%03i.jpg", found);
          FILE *outptr = fopen(outfile, "w");
          outfile_opened = outptr;
        }

        if (found > found_prev)
        {
          //
          fwrite(one_box, 512, 1, outfile_opened);
        }

        // free the one_box
        free(one_box);

    }  // head loop ends

    // free card
    free(card);

    return 0;  // main on success
}
