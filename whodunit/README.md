# Questions

## What's `stdint.h`?

It is the header file includes defination of integer types of fixed width.

## What's the point of using `uint8_t`, `uint32_t`, `int32_t`, and `uint16_t` in a program?

It is to assign the memory or data limit of the user defined datatype in bmp.h, wher the user(more precisely developer) is Microsoft perhaps.

## How many bytes is a `BYTE`, a `DWORD`, a `LONG`, and a `WORD`, respectively?

1, 4, 4, 2

## What (in ASCII, decimal, or hexadecimal) must the first two bytes of any BMP file be? Leading bytes used to identify file formats (with high probability) are generally called "magic numbers."

hexadecimal

## What's the difference between `bfSize` and `biSize`?

bfSize is the total size of the BMP file, including the the two headers and the image. But the biSize is the dimenstion of the bitmap image only.

## What does it mean if `biHeight` is negative?

If biHeight is negative, the bitmap is a top-down DIB with the origin at the upper left corner.

## What field in `BITMAPINFOHEADER` specifies the BMP's color depth (i.e., bits per pixel)?

biBitCount

## Why might `fopen` return `NULL` in lines 24 and 32 of `copy.c`?

It might be because the file fopen trying to open might not have anything inside it. So it will return a NULL pointer which really points to nothing.

## Why is the third argument to `fread` always `1` in our code?

In our code we're reading one by one triple of a BMP file, that's why third argument is 1 which denotes the number of things to read from the file.

## What value does line 63 of `copy.c` assign to `padding` if `bi.biWidth` is `3`?

3

## What does `fseek` do?

It can place or more conveniently saying move the cursor/pointer to the location of the file as desired.

## What is `SEEK_CUR`?

It is a argument of fseek function saying that the cursor to move accourding to the second argument of fseek function.