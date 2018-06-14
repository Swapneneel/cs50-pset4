// This will recover the deleted jpegs from the memory card

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// the defiition of struct
#include <stdint.h>

typedef uint8_t BYTE;  // it was uint not unit == > uint stands for unsigned integer

typedef struct
{
    BYTE byte_each;
} __attribute__((__packed__))
JPEG;

// definition of struct ended

int main(int argc, char *argv[])
{
    // ensuring only one argument the card.raw can be entered
    if (argc != 2)
    {
        fprintf(stderr, "Useage: ./recover card.raw\n");
        return 1;
    }


    // open input file
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        fprintf(stderr, "Counld not open the %s.raw file.\n", argv[1]);
        return 2;
    }  // upto ok



    int found = 0;

    char outfile[8];  // file name char array
    FILE *img = NULL;  // outfile pointer name

    JPEG *one_box = malloc(sizeof(JPEG) * 512);  // one box with size 512 bytes
    if (one_box == NULL)  // show error if asked memory not available
    {
        fprintf(stderr, "The memory required is not available.\n");
        printf("The memory required is not available.\n");
        return 3;
    }

    // till the end of the file card
    while (fread(one_box, sizeof(one_box), 1, card) > 0)  // fread on successfull transection returns no of files read
    {
        // this will check all the possible JPEG header options
        if (one_box[0].byte_each == 0xff && one_box[1].byte_each == 0xd8 &&
            one_box[2].byte_each == 0xff && (one_box[3].byte_each & 0xf0) == 0xe0)
        {
            // it will count up when a new JPEG is found
            found++;

            // close the file previously opened
            if (found > 1)
            {
                fclose(img);
            }

            // file name & file opening for reading
            sprintf(outfile, "%03i.jpg", (found - 1));
            img = fopen(outfile, "a");  // no need to put the * sign, only write the variable name
            if (img == NULL)
            {
                // error message for not opening the outfile
                fprintf(stderr, "Could not create %s.\n", outfile);
                return 4;
            }
            // write 512 byte at once
            fwrite(one_box, sizeof(one_box), 1, img);

        }
        else
        {
            if (found > 0)  // it will write if a file is already opened
            {
                fwrite(one_box, sizeof(one_box), 1, img);  // 512 byte each time
            }
        }

    }
    // free img
    free(img);

    // close the card
    fclose(card);

    return 0;  // main on success
}
