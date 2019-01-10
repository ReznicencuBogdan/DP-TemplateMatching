/*
    Purpose: Implements the bitmap parser
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#ifndef BMP_H
#define BMP_H

#include "datatypes.h"
#include "geometrics.h"
  /*
  * @pragma pack used to disble annoying | useful padding */
  #pragma pack(push, 1)
  typedef struct        {
    unsigned char       checksum[2];
    dword               size;
    unsigned short int  reserved1;
    unsigned short int  reserved2;
    dword               imageDataOffset;
    dword               biSize;
    dword               width;
    dword               height;
    unsigned short int  planes;
    unsigned short int  pxBits;
    dword               compression;
    dword               sizeWithPadding;
    dword               hrPxPm;
    dword               vrPxPm;
    dword               clrn;
    dword               importantClrs;
  } bmpheader;

  typedef struct {
    color b;
    color g;
    color r;
  } pixel;

  typedef struct {
    bmpheader header;
    pixel *imagedata;
  } bitmap;

  typedef struct {
    dword R0;
    dword SV;
  } encryptionKey;
  #pragma pack(pop)


  // defining some colors
  #define C_RED     "\x1b[31m"
  #define C_GREEN   "\x1b[32m"
  #define C_YELLOW  "\x1b[33m"
  #define C_BLUE    "\x1b[34m"
  #define C_MAGENTA "\x1b[35m"
  #define C_CYAN    "\x1b[36m"
  #define C_RESET   "\x1b[0m"


/*****************************************************************************
******************************************************************************
***********  Function definitions used for image manipulation  ***************
******************************************************************************
*****************************************************************************/

/*
* @rgb returns a pixel with colors set as wanted */
pixel rgb(color r, color g, color b);

/*
* @setBitmapPixelAt sets a pixel at coordinates.
* Note: It assumes that parameters are valid! */
ntvoid setBitmapPixelAt(bitmap *bmp, point coord, pixel pxl);

/*
* @getBitmapPixelAt gets a pixel at coordinates.
* Note: It assumes that parameters are valid! */
pixel getBitmapPixelAt(bitmap *bmp, point coord);

/*
* @drawBitmapLine will draw a line petween the 2 points provided
* Note: It assumes that parameters are valid! */
ntvoid drawBitmapLine(bitmap *bmp, point startp, point endp, pixel pxl);

/*
* @drawBitmapRect draws a rectangle at specified coordinates
* NOTE: It assumes that the parameters are valid! */
ntvoid drawBitmapRect(bitmap *bmp, rect rc, pixel pxl);

/*
* @computeBmpPadding calculates the padding needed for the line*/
dword computeBitmapPadding(dword width);

/*
* @readBitmap does the same thing as @readBmpLinearized. This function exists solely for readability. */
ntstatus readBitmap(const char * path, bitmap *bmp);

/*
* @readBmpLinearized will construct a pixel array using the header structure provided */
ntstatus readBitmapLinearized(const char * path, bitmap *bmp);

/*
* @writeBmp does the same thing as @writeBmpLinearized. This function exists solely for readability. */
ntstatus writeBitmap(const char * path, const bitmap *bmp);

/*
* @writeBmpLinearized will dump the header and pixel data into a new binary */
ntstatus writeBitmapLinearized(const char *path, const bitmap *bmp);

/*
* @freeBitmap will free the bitmap loaded in memory */
ntstatus freeBitmap(bitmap *bmp);

/*
* @isBmpValid will check the validity of the bmp file provided */
ntstatus isBitmapValid(const bitmap *bmp);

/*
* @xorPixelWithPixel returns a pixel resulted from the xor of the pixels provided */
pixel xorPixelWithPixel(pixel pixel1, pixel pixel2);

/*
* @xorPixelWithUint32 returns pixel obtained from xoring the pixel with the 3 least bytes of the unsigned provided */
pixel xorPixelWithUint32(pixel pixel1, dword pixel2);

/*
* @copyBmpHeader copies the header from source/bmp2 to destination/bmp1 */
ntstatus copyBitmapHeader(bitmap *bmp1, bitmap *bmp2);

/*
* @readBmpEncryptionKey reads the encryption keys from path into the struct 'key' */
ntstatus readBitmapEncryptionKey(const char *keypath, encryptionKey *key);

/*
* @encryptBmp encrypt the bitmap in path unsing the algorithms in algorithm.h. Encryption key is found int key_path */
ntstatus encryptBitmap(const char *path, const char *dest_path, const char *key_path);

/*
* @decryptBmp decrypts the bitmap in path unsing the algorithms in algorithm.h. Decryption key is found int key_path */
ntstatus decryptBitmap(const char *path, const char *dest_path, const char *key_path);

/*
* @chiSquareTest shows the chi tests for each pixel channel */
ntstatus chiSquareTest(const char *path);

/*
* @cvrtBmpGrayscale converts the given bitmap to grayscale */
ntstatus cvrtBitmapGrayscale(bitmap *bmp);
#endif
