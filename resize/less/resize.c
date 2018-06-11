//Resize a BMP file
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"


int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Useage: resize scale infile outfile.\n");
        return 1;
    }
    else
    {
        //Checking the scale as on specification
        int n = atoi(argv[1]);

        if (n < 0 && n < 100)
        {
            fprintf(stderr, "Useage: resize scale should be positive upto 100.\n");
            return 1;
        }
        /*
        else if (isBMP(argv[2]))
        {
            //Checking the infile is a BMP or not
            fprintf(stderr, "Useage: %s is not a BMP.\n", argv[2]);
            return 1;
        }
        else if (isBMP(argv[3]))
        {
            //checking the outfile is a name of BMP
            fprintf(stderr, "Useage: %s is not a name of BMP file.\n", argv[3]);
            return 1;
        }
        */

    }  //Input checking is over

    //remember scale & file names
    int scale = atoi(argv[1]);
    char *infile = argv[2];
    char *outfile = argv[3];

    //open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    //open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    //read BITMAPFILEHEADER of infile
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    //read BITMAPINFOHEADER of infile
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //ensure the infile is a 24-bit uncompressed BPM 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        fprintf(stderr, "Unsupported file format %s\n", infile);
        return 4;
    }

    // remember infile's BITMAPINFOHEADER for iteration, as it is going to change for the outfile
    int originalWidth = bi.biWidth;
    int originalHeight = bi.biHeight;

    // determine padding for iterating over scanline (i.e. infile's padding)
    int padding_in = (4 - (originalWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    // determine padding for the outfile
    int padding_out = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // updating the BITMAPINFOHEADER according to outfile
    bi.biWidth *= scale;  // Width of outfile = n * infile
    bi.biHeight *= scale;  // Height of outfile = n * infile

    // biSizeImage is the total size of the image in bytes including paddings
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_out) * abs(bi.biHeight);

    // updating BITMAPFILEHEADER according to outfile
    // bfSize is the total size of the file including total size of image & file headers
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);


    // these are writing of outfile headers
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    /******************************
     * Trying to implement rewrite method for resizing the BMP
     * Not wprking properly yet.May be the method is not yet properly yet. Let's see.
     **********************************/

    // iterate over infiles's scanline (Vertically)
    for (int i = 0, bi_Height = abs(originalHeight); i < bi_Height; i++)
    {
        RGBTRIPLE *arr = malloc(sizeof(RGBTRIPLE) * (bi.biWidth));  // this key thing I did wrong
        if (arr == NULL)
        {
            fprintf(stderr, "The memory required is not available.\n");
            printf("The memory required is not available.\n");
            return 5;
        }

        int tracker = 0;
        // iterate over pixals in scanline (Horizontally)
        for (int j = 0; j < originalWidth; j++)
        {
            //Pixle combination
            RGBTRIPLE triple;

            // reading RGBTRIPLE from infile(basically copying it)
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // for each pixel writing to the array 'n' times
            for (int n = 0; n < scale; n++)
            {
                *(arr + (tracker)) = triple;  // using pointer arithmatic
                tracker++;
            }
        }
        // writing the arr to outfile for 'n' times
        for (int m = 0; m < scale; m++)
        {
            fwrite((arr), sizeof(RGBTRIPLE), scale, outptr);  // it should include the triples
                          // as according to scale
            // add the padding to outfile as of scale (to demonstrate how)
            for (int k = 0; k < padding_out; k++)
            {
                fputc(0x00, outptr);
            }
        }

        // skip over padding, if any (in the infile, to prepare to move to the next row)
        fseek(inptr, padding_in, SEEK_CUR);

        //}

        free(arr);  // freeying the memory
    }


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}

/*
bool isBMP(char *file_name)
{
    if (file_name)
}
*/
