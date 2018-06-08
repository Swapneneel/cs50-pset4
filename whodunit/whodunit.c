#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])  //Removed the training wheels of String
{
    //Exactly two commandline arguments
    if (argc != 3)
    {
        fprintf(stderr, "Useage: copy infile outfile\n");
        return 1;
    }

    //The filenames to be used
    char *infile = argv[1];
    char *outfile = argv[2];

    //Opening the infile to read
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    //Opening the outfile to write
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    //Reading infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    //Reading infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //Now checking for infile a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40
        || bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }

    //Writing outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    //Writting outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    //Determine padding for scanline
    int padding = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    //Iterate over infile's scanline
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        //iterate over scanline pixle by pixle
        for (int j = 0; j < bi.biWidth; j++)
        {
            //Temporaty storage
            RGBTRIPLE triple;

            //Read triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            //The stratergy is to to convert all the pure red to white
            if (triple.rgbtBlue == 0x00 && triple.rgbtGreen == 0x00 && triple.rgbtRed == 0xff)
            {
                triple.rgbtBlue = 0xff;
                triple.rgbtGreen = 0xff;
                triple.rgbtRed = 0xff;
            }

            //Write RGB triple to the outfile
            fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
        }

        //Skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);

        //then add it back to demonstrate how
        for (int k = 0; k < padding; k++)
        {
            fputc(0x00, outptr);
        }
    }

    //close infile
    fclose(inptr);

    //close outfile
    fclose(outptr);

    //success
    return 0;

}