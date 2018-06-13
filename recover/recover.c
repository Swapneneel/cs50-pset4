// This will recover the deleted jpegs from the memory card

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// the defiition of struct
#include <stdint.h>

typedef uint8_t BYTE;  // it was uint not unit == > uint stands for unsigned integer

typedef struct {
    BYTE byte_each;
} __attribute__ ((__packed__))
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
    FILE *inptr = fopen(argv[1], "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Counld not open the %s.raw file.\n", argv[1]);
        return 2;
    }  // upto ok



    int found = 0;

    char *outfile = NULL;  // file name string
    FILE *outptr_opened = NULL;  // outfile pointer name

    fseek(inptr, 0L, SEEK_END);
    long int n = ftell(inptr) / 512;
    // printf("n = %li\n", n);

    fseek(inptr, 0L, SEEK_SET);

    for (long int i = 0; i < n; i++)
    {
        JPEG *one_box = malloc(sizeof(JPEG) * 512);  // one box with size 512 bytes
        if (one_box == NULL)  // show error if asked memory not available
        {
            fprintf(stderr, "The memory required is not available.\n");
            printf("The memory required is not available.\n");
            return 3;
        }

        fread(one_box, sizeof(JPEG), 512, inptr);

        if (one_box[0].byte_each == 0xff && one_box[1].byte_each == 0xd8 &&
               one_box[2].byte_each == 0xff && (one_box[3].byte_each & 0xf0) == 0xe0)
        {
              found++;

            // close the file previously opened
            if (found > 1)
            {
                fclose(outptr_opened);
            }

            // file name & file opening for reading
            sprintf(outfile, "%03i.jpg", (found - 1));
            FILE *outptr = fopen(outfile, "w");  // no need to put the * sign, only write the variable name
            if (outptr == NULL)
            {
                // error message for not opening the outfile
                fprintf(stderr, "Could not create %s.\n", outfile);
                return 3;
            }
            *outptr = *outptr_opened;
        }

        if (found > 0)  // wtite only when after found the 1st one
        {
            fwrite(one_box, sizeof(JPEG), 512, outptr_opened);  // 512 byte each time
        }

        // fseek(inptr, 512, SEEK_CUR);

        // free the alocated memory
        free(one_box);
    }

    // free one_box
    //free(one_box);

    fclose(inptr);

    return 0;  // main on success
}
