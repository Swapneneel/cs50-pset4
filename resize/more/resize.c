// Resizing the the bmp with float scale

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "bmp.h"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Useage: resize scale infile outfile.\n");
        return 1;
    }

    // convert to float
    float scale = atof(argv[1]);
    // verifying the scale accourding to specs
    if (scale < 0.00 || scale > 100.00)  // the case is not expected
    {
        fprintf(stderr, "Useage: %s should be between 0.0 and 100.0", argv[1]);
        return 1;
    }

    // other two inputs are unchecked return 1 if error

    // open infile and outfile
    FILE *inptr = fopen(argv[2], "r");
    if (inptr == NULL)  // error check
    {
        fprintf(stderr, "Could not oprn %s\n", argv[2]);
        return 2;
    }

    FILE *outptr = fopen(argv[3], "w");
    if (outptr == NULL)  // error check
    {
        fprintf(stderr, "Couldn't create %s\n", argv[3]);
        return 2;
    }

    // red BITMAPFILEHEADER of infile
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read BITMAPINFOHEADER of infile
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    //ensure the infile is a 24-bit uncompressed BPM 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(inptr);
        fclose(outptr);
        fprintf(stderr, "Unsupported file format %s\n", argv[2]);
        return 4;
    }

    // remember the infile's BITMAPINFOHEADER data so that to iterate accoding
    int originalWidth = bi.biWidth;
    int originalHeight = bi.biHeight;

    // determine the padding of the infile
    int padding_in = (4 - (originalWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // modifying the Height & Width for outfile
    bi.biWidth = floor(scale * bi.biWidth);  // modification
    // Try to consider the case of Bottom-Up
    bi.biHeight = floor(scale * (bi.biHeight > 0? -bi.biHeight : bi.biHeight));  // floor() to used, not to exceed the non-decimal value
    //---------------------------------++++++++++++++++++------------------------------------

    // determine padding for the outfile
    int padding_out = (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;

    // biSizeImage is the total size of the image in bytes including paddings
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + padding_out) * abs(bi.biHeight);

    // updating BITMAPFILEHEADER according to outfile
    // bfSize is the total size of the file including total size of image & file headers
    bf.bfSize = bi.biSizeImage + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // these are writing of outfile headers
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);


    // Code for downscaling
    if (scale < 1.00)
    {
        // Calulating which of them to select
        int h = round(1 / scale);
        // int v = (int) (originalHeight / bi.biHeight);

        // only selecting the scanline which is going to resultent
        for (int i = 0, bi_Height = abs(originalHeight); i < bi_Height; i++)
        {
            for (int j = 0; j < originalWidth; j++)
            {
                RGBTRIPLE triple;

                // read the pixel to be used
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write the scanlines as many as need
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);

                // skip the rejected triple
                fseek(inptr, sizeof(RGBTRIPLE) * (h - 1), SEEK_CUR);
            }

            // skip over padding, if any (in the infile, to prepare to move to the next row)
            fseek(inptr, padding_in, SEEK_CUR);

            // add the padding again to the outptr file
            for (int k = 0; k < padding_out; k++)
            {
                fputc(0x00, outptr);
            }

            // skip the rejected horizontal line
            fseek(inptr, ((sizeof(RGBTRIPLE) * (originalWidth * (h - 1))) + padding_in), SEEK_CUR);
        }
    }

    // Upscaling
    else if (scale > 1.00)
    {
        // Calulating which of them to select
        scale = round(scale);

        // only selecting the scanline which is going to resultent
        for (int i = 0, bi_Height = abs(originalHeight); i < bi_Height; i++)
        {
            for (int j = 0; j < originalWidth; j++)
            {
                RGBTRIPLE triple;

                // read the pixel to be used
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // write the scanlines as many as need
                for (int m = 0; m < scale; m++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }

            // skip over padding, if any (in the infile, to prepare to move to the next row)
            fseek(inptr, padding_in, SEEK_CUR);

            // add the padding again to the outptr file
            for (int k = 0; k < padding_out; k++)
            {
                fputc(0x00, outptr);
            }

            // skip the rejected horizontal line
            fseek(inptr, ((sizeof(RGBTRIPLE) * originalHeight) + padding_in), SEEK_CUR);
        }
    }

    // For same size, copy and paste
    else
    {
        // scanning through the entire scaline
        for (int i = 0, bi_Height = abs(originalHeight); i < bi_Height; i++)
        {
            for (int j = 0; j < originalWidth; j++)
            {
                RGBTRIPLE triple;

                // read the pixel form infile
                fread(&triple, sizeof(RGBTRIPLE), 1 , inptr);

                // write the pixel to outfile
                fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
            }

            // skip over padding, if any (in the infile, to prepare to move to the next row)
            fseek(inptr, padding_in, SEEK_CUR);

            // add the padding again to the outptr file
            for (int k = 0; k < padding_out; k++)
            {
                fputc(0x00, outptr);
            }
        }
    }


    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);



    return 0;
}

