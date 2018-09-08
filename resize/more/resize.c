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
    bi.biWidth = scale * bi.biWidth;  // modification
    // Try to consider the case of Bottom-Up
    bi.biHeight = scale * bi.biHeight;  // floor() to used, not to exceed the non-decimal value
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
        float f, fi, fStep, sY1, sY2, jstart, jend, sX1, sX2, istart, iend, dy, dx, area;

        // calculating set "A"
        f = 1 / scale;
        fi = scale * scale;
        fStep = f * 0.9999;

        // along destination height
        for (int y = 0, des_Height = abs(bi.biHeight); y < des_Height; y++)
        {
            // calculate set "B"
            sY1 = y * f;
            sY2 = sY1 + fStep;
            jstart = floor(sY1);
            jend = ceil(sY2) - 1;

            // along destination width
            for (int x = 0; x < bi.biWidth; x++)
            {
                // calculate set "C"
                sX1 = x * f;
                sX2 = sX1 + fStep;
                istart = floor(sX1);
                iend = ceil(sX2) - 1;

                // defining the output file one pixel
                RGBTRIPLE triple, pixel;

                // along original height
                for (float j = jstart; j < jend; j++)
                {
                    // deciding the size of output pixel, y co-ordinate
                    dy = 1;
                    if (sY1 > j)
                    {
                        dy = dy - (sY1 - j);
                    }
                    if (sY2 < j + 1)
                    {
                        dy = dy - (j + 1 - sY2);
                    }

                    // along original width
                    for (float i = istart; i < iend; i++)
                    {
                        // deciding the size of output pixel, x co-ordinate
                        dx = 1;
                        if (sX1 > i)
                        {
                            dx = dx - (sX1 - i);
                        }
                        if (sX2 < i + 1)
                        {
                            dx = dx - (i + 1 - sX2);
                        }

                        // area of the destination pixel
                        area = dx * dy * fi;

                        // reading the triple from original file's section
                        fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                        // here modifications for all the colours will be done
                        pixel.rgbtBlue += trunc(triple.rgbtBlue * area);
                        pixel.rgbtRed += trunc(triple.rgbtRed * area);
                        pixel.rgbtGreen += trunc(triple.rgbtGreen * area);
                    }
                }

                // writing the pixel to the destination file
                fwrite(&pixel, sizeof(RGBTRIPLE), 1, outptr);
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

    // Upscaling
    else if (scale > 1.00)
    {
        // memory allocation for array
        RGBTRIPLE *buff = malloc(sizeof(RGBTRIPLE) * (bi.biWidth));

        // setting up the counter
        int count = 0;

        // looping over the infile's height
        for (int j = 0, bi_Height = abs(originalHeight); j < bi_Height; j++)
        {
            // looping over the infile's height
            for (int i = 0; i < originalWidth; i++)
            {
                RGBTRIPLE triple;

                // read the triple
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

                // coppying the pixel in the buffer
                for (int m = 0; m < scale; m++)
                {
                    buff[count] = triple;
                    count++;
                }
            }

            // skeep the infile padding
            fseek(inptr, padding_in, SEEK_CUR);

            // writing the rows in the outfile
            for (int n = 0; n < scale; n++)
            {
                // writing the all the pixels to the outfile row
                fwrite(buff, sizeof(RGBTRIPLE), bi.biWidth, outptr);

                // adding the padding agian
                for (int k = 0; k < padding_out; k++)
                {
                    fputc(0x00, outptr);
                }
            }
        }

        // free the alocated memory
        free(buff);
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
