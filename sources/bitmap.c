/*
    Purpose: Implements the bitmap parser
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "message.h"
#include "bitmap.h"
#include "algorithms.h"

/*********************************************************************************
**********************************************************************************
***********  Function implementations used for image manipulation  ***************
**********************************************************************************
*********************************************************************************/

pixel rgb(color r, color g, color b)
{
  pixel pxl;
  pxl.r = r;
  pxl.g = g;
  pxl.b = b;

  return pxl;
}






ntvoid setBitmapPixelAt(bitmap *bmp, point coord, pixel pxl)
{
  bmp->imagedata[coord.y * bmp->header.width + coord.x] = pxl;
}







pixel getBitmapPixelAt(bitmap *bmp, point coord)
{
  return bmp->imagedata[coord.y * bmp->header.width + coord.x];
}





ntvoid drawBitmapLine(bitmap *bmp, point startp, point endp, pixel pxl)
{
  /*
  * NOTE: used here an elegant aproach: bresenham's algorithm.
  *       so that I do not have to write usseles cases for drawing verticaly|horizontally|angled.
  */

  int dx = abs(endp.x-startp.x);
  int sx = startp.x < endp.x ? 1 : -1;
  int dy = abs(endp.y-startp.y);
  int sy = startp.y < endp.y ? 1 : -1;
  int err = (dx>dy ? dx : -dy)/2, e2;

  for(;;){
    setBitmapPixelAt(bmp, startp, pxl);

    if (startp.x==endp.x && startp.y==endp.y) break;
    e2 = err;

    if (e2 >-dx) {
      err -= dy;
      startp.x += sx;
    }
    if (e2 < dy) {
      err += dx;
      startp.y += sy;
    }
  }
}






ntvoid drawBitmapRect(bitmap *bmp, rect rc, pixel pxl)
{
  /*
  *   MATHEMATICAL POINT OF VIEW: this code is not optimised.(yet).
  *   Can be faster. Will work at it in future release.
  *   TODO: follow request
  */
  point auxiliary;

  // draw top line
  setPointCoordinates(&auxiliary, rc.rd.x, rc.lu.y);
  drawBitmapLine(bmp, rc.lu, auxiliary, pxl);

  // draw bottom line
  setPointCoordinates(&auxiliary, rc.lu.x, rc.rd.y);
  drawBitmapLine(bmp, auxiliary, rc.rd, pxl);

  // draw left vertical line
  drawBitmapLine(bmp, rc.lu, auxiliary, pxl);

  // draw right vertical line
  setPointCoordinates(&auxiliary, rc.rd.x , rc.lu.y);
  drawBitmapLine(bmp, auxiliary, rc.rd, pxl);
}






dword computeBitmapPadding(dword width)
{
  dword padding = 0;

  if(width % 4 != 0)
      padding = 4 - (3 * width) % 4;

  return padding;
}




ntstatus readBitmap(const char * path, bitmap *bmp)
{
  return readBitmapLinearized(path, bmp);
}






ntstatus readBitmapLinearized(const char * path, bitmap *bmp)
{
  if(bmp == NULL) {
    debugMessage("readBitmapLinearized: error bmp is null.");
    return false;
  }

  // do some cleanup
  bmp->imagedata = NULL;

  FILE * lpHandle = fopen(path, "rb");

  if(lpHandle == NULL)  {
    debugMessage("readBitmapLinearized: error lpHandle is null.");
    return false;
  }

  /*
  * This function may lead to some memory leaks if the programmer doesn't know that
  *   the address stored in bmp->imagedata will be changed.
  * There is no other way to circumvent programmer idiocy.
  */

  // read bitmap header
  fread(&(bmp->header), sizeof(bmp->header), 1, lpHandle);


  // compute some offsets
  dword  padding = computeBitmapPadding(bmp->header.width);
  qword  ubound = bmp->header.width * bmp->header.height;
  qword  offset = ubound;


  // allocate enough space to hold the rgb structs
  bmp->imagedata = (pixel*)malloc(ubound * sizeof(pixel));



  // if there was an error allocating the memory
  if(bmp->imagedata == NULL){
    fclose(lpHandle);

    debugMessage("readBitmapLinearized: error bmp->imagedata is null.");
    return false;
  }

  /*
  * NOTE: notice that I am reading the image from the bottom upwards. So first pixel in last line will be
  *       first pixel on column 0 in the array.*/
  for(dword i = 0;i<bmp->header.height; i++) {
    // recalculate the offset
    offset -= bmp->header.width;

    // read the pixels in the vector at offset
    fread(bmp->imagedata + offset,sizeof(pixel), bmp->header.width, lpHandle);

    // skip the padding
    fseek(lpHandle, padding, SEEK_CUR);
  }

  fclose(lpHandle);
  return true;
}






ntstatus writeBitmap(const char *path, const bitmap *bmp)
{
  return writeBitmapLinearized(path, bmp);
}






ntstatus writeBitmapLinearized(const char *path, const bitmap *bmp)
{
  if(path == NULL || bmp->imagedata == NULL) {
    debugMessage("writeBitmapLinearized: error path or bmp->imagedata are null. ");
    return false;
  }

  FILE *lpHandle = fopen(path, "wb+");

  if(lpHandle==NULL) {
    debugMessage("writeBitmapLinearized: error lpHandle is null.");
    return false;
  }

  dword  padding = computeBitmapPadding(bmp->header.width);
  color  garbage[5] = {0};
  qword  offset = bmp->header.width * bmp->header.height;;

  // write coressponding header
  fwrite(&(bmp->header), sizeof(bmp->header), 1, lpHandle);

  /*
  * NOTE: notice that I am writing the same way I am reading.*/
  for(dword i=0;i<bmp->header.height;i++){
     // recalculate the offset
     offset -= bmp->header.width;

     // writes the entire pixel line to file
     fwrite(bmp->imagedata + offset, sizeof(pixel), bmp->header.width, lpHandle);

     // writes the necessary padding
     // NOTE: for some reason it looks like I cannot fseek. If I don't write something after fseek the the cursor in the file won't move.
     fwrite(garbage, padding,1, lpHandle);
  }

  fclose(lpHandle);
  return true;
}






ntstatus freeBitmap(bitmap *bmp)
{
  if(bmp == NULL) {
    debugMessage("freeBitmap: error bmp is null.");
    return false;
  }

  if(bmp->imagedata == NULL) {
    debugMessage("freeBitmap: error bmp->imagedata is null.");
    return false;
  }

  free(bmp->imagedata);

  return true;
}





ntstatus isBitmapValid(const bitmap *bmp)
{
  if(bmp == NULL){
    debugMessage("isBitmapValid: error bmp is null.");
    return false;
  }

  if(!(bmp->header.checksum[0] == 0x42  && bmp->header.checksum[1] == 0x4d))
    return false;

  return true;
}





pixel xorPixelWithPixel(pixel pixel1, pixel pixel2)
{
  pixel result;

  result.r = (pixel1.r) ^ (pixel2.r);
  result.g = (pixel1.g) ^ (pixel2.g);
  result.b = (pixel1.b) ^ (pixel2.b);

  return result;
}




pixel xorPixelWithUint32(pixel pixel1, dword _data_2)
{
  /*
  * Not necessary(if image was saved in other endianne then it would be usseles anyway)
  */

  unsigned char *_pbytes = (unsigned char*)&_data_2;
  pixel result;

  if(getEndian() == little_endian){
    result.r = (pixel1.r) ^ _pbytes[2];
    result.g = (pixel1.g) ^ _pbytes[1];
    result.b = (pixel1.b) ^ _pbytes[0];
  }else {
    result.r = (pixel1.r) ^ _pbytes[1];
    result.g = (pixel1.g) ^ _pbytes[2];
    result.b = (pixel1.b) ^ _pbytes[3];
  }

  return result;
}





ntstatus copyBitmapHeader(bitmap *bmp1, bitmap *bmp2)
{
  if(bmp1 == NULL || bmp2 == NULL) {
    debugMessage("copyBitmapHeader: parameter are invalid.");
    return false;
  }

  memcpy(&(bmp1->header), &(bmp2->header), sizeof(bmp2->header));
  return true;
}




ntstatus readBitmapEncryptionKey(const char *keypath, encryptionKey *key)
{
  FILE * lpHandle = fopen(keypath, "r");

  if(lpHandle == NULL) {
    debugMessage("readBitmapEncryptionKey: error lpHandle is null");
    return false;
  }

  if(key == NULL) {
    debugMessage("readBitmapEncryptionKey: error key is null");
    fclose(lpHandle);
    return false;
  }

  fscanf(lpHandle, "%u %u", &key->R0, &key->SV);
  fclose(lpHandle);
  return true;
}




ntstatus encryptBitmap(const char *path, const char *dest_path, const char *key_path)
{
  encryptionKey key;
  bitmap original_image;
  bitmap encrypted_image;
  dword *random_sequence;
  dword *permutation_sequence;
  qword ubound;


  // read the encryption key stored in file
  if(!readBitmapEncryptionKey(key_path, &key)) {
    debugMessage("encryptBitmap: failed reading encryption key.");
    return false;
  }

  // read the bitmap
  if(!readBitmapLinearized(path, &original_image)) {
    debugMessage("encryptBitmap: failed reading bitmap to encrypt.");
    return false;
  }

  // copy header from original bitmap
  copyBitmapHeader(&encrypted_image, &original_image);

  // compute the total image size in pixels
  ubound = original_image.header.width * original_image.header.height;

  if(!generateRandomSequence(key.R0, ubound * 2, &random_sequence)){
    debugMessage("encryptBitmap: failed generating random_sequence.");

    free(original_image.imagedata);
    return false;
  }

  if(!generatePermutation(ubound, random_sequence, &permutation_sequence)) {
    debugMessage("encryptBitmap: failed generating permutation_sequence.");

    free(original_image.imagedata);
    free(random_sequence);
    return false;
  }

  if(!generatePermuttedSequence(permutation_sequence, ubound, (void*)original_image.imagedata, (void*)&(encrypted_image.imagedata), sizeof(pixel))) {
    debugMessage("encryptBitmap: failed applying permutation.");

    free(original_image.imagedata);
    free(random_sequence);
    free(permutation_sequence);
    return false;
  }

  // no longer needeed
  free(permutation_sequence);
  free(original_image.imagedata);

  // substitute the pixel data
  for(qword k=0;k<ubound;k++){
    if(k==0)
      encrypted_image.imagedata[0] = xorPixelWithUint32(encrypted_image.imagedata[0], key.SV ^ random_sequence[ubound]);
    else {
      encrypted_image.imagedata[k] = xorPixelWithPixel(encrypted_image.imagedata[k-1], encrypted_image.imagedata[k]);
      encrypted_image.imagedata[k] = xorPixelWithUint32(encrypted_image.imagedata[k] , random_sequence[ubound + k]);
    }
  }

  // no longer needed this random sequence
  free(random_sequence);

  if(!writeBitmapLinearized(dest_path, &encrypted_image)){
    debugMessage("encryptBitmap: failed writing encrypted bitmap.");

    free(encrypted_image.imagedata);
    return false;
  }

  free(encrypted_image.imagedata);
  return true;
}



ntstatus decryptBitmap(const char *path, const char *dest_path, const char *key_path)
{
  bmpheader header;
  encryptionKey key;
  bitmap encrypted_image;
  bitmap decrypted_image;
  dword *random_sequence;
  dword *permutation_sequence;
  qword ubound;

  // read the decryption key stored in file
  if(!readBitmapEncryptionKey(key_path, &key)){
    debugMessage("decryptBitmap: failed reading encryption key.");
    return false;
  }

  // read the bitmap
  if(!readBitmapLinearized(path, &encrypted_image)) {
    debugMessage("decryptBitmap: failed reading encrypted bitmap.");
    return false;
  }

  // copy the original header into the new decrypted image
  copyBitmapHeader(&decrypted_image, &encrypted_image);

  // compute the total image size in pixels
  ubound = encrypted_image.header.width * encrypted_image.header.height;

  if(!generateRandomSequence(key.R0, ubound * 2, &random_sequence)){
    debugMessage("decryptBitmap: failed generating random_sequence.");

    free(encrypted_image.imagedata);
    return false;
  }

  if(!generatePermutation(ubound, random_sequence, &permutation_sequence)) {
    debugMessage("decryptBitmap: failed generating permutation_sequence.");

    free(encrypted_image.imagedata);
    free(random_sequence);
    return false;
  }

  if(!inversePermutation(ubound, &permutation_sequence)){
    debugMessage("decryptBitmap: failed generating the inverse of permutation.");

    free(encrypted_image.imagedata);
    free(random_sequence);
    free(permutation_sequence); // because the inverse hasn't been replaced or freed
    return false;
  }

  // decrypting the pixel data
  for(qword k=0;k<ubound;k++){
    if(k==0)
      encrypted_image.imagedata[0] = xorPixelWithUint32(encrypted_image.imagedata[0], key.SV ^ random_sequence[ubound]);
    else {
      encrypted_image.imagedata[k] = xorPixelWithPixel(encrypted_image.imagedata[k-1], encrypted_image.imagedata[k]);
      encrypted_image.imagedata[k] = xorPixelWithUint32(encrypted_image.imagedata[k] , random_sequence[ubound + k]);
    }
  }

  if(!generatePermuttedSequence(permutation_sequence, ubound, (void*)encrypted_image.imagedata, (void*)&decrypted_image.imagedata, sizeof(pixel))){
    debugMessage("decryptBitmap: failed applying permutation on sequence.");

    free(encrypted_image.imagedata);
    free(random_sequence);
    free(permutation_sequence);
    return false;
  }

  free(encrypted_image.imagedata);
  free(random_sequence);
  free(permutation_sequence);

  if(!writeBitmapLinearized(dest_path, &decrypted_image)){
    debugMessage("decryptBitmap: failed writing decrypted image.");

    free(decrypted_image.imagedata);
    return false;
  }

  free(decrypted_image.imagedata);
  return true;
}



ntstatus chiSquareTest(const char *path)
{
  bitmap bmp;

  if(!readBitmap(path, &bmp)) return false;


  unsigned long long rFrq[256];
  unsigned long long gFrq[256];
  unsigned long long bFrq[256];

  double ubound = bmp.header.width * bmp.header.height;
  double estimatedFrq = (double)ubound / 256;
  qword k;

  memset(rFrq,0x00,sizeof(rFrq));
  memset(gFrq,0x00,sizeof(gFrq));
  memset(bFrq,0x00,sizeof(bFrq));

  for(k=0;k<ubound;k++){
    rFrq[bmp.imagedata[k].r]++;
    gFrq[bmp.imagedata[k].g]++;
    bFrq[bmp.imagedata[k].b]++;
  }

  freeBitmap(&bmp);

  double chiSqR=0;
  double chiSqG=0;
  double chiSqB=0;

  for(dword i=0;i<256;i++) {
    chiSqR += (rFrq[i] - estimatedFrq) * (rFrq[i] - estimatedFrq) / estimatedFrq;
    chiSqG += (gFrq[i] - estimatedFrq) * (gFrq[i] - estimatedFrq) / estimatedFrq;
    chiSqB += (bFrq[i] - estimatedFrq) * (bFrq[i] - estimatedFrq) / estimatedFrq;
  }

  printf("\n Red channel:%f\n Green channel:%f\n Blue Channel:%f\n\n", chiSqR, chiSqG, chiSqB);
  return true;
}



ntstatus cvrtBitmapGrayscale(bitmap *bmp) {
  if(bmp == NULL || bmp->imagedata == NULL) {
    debugMessage("cvrtBitmapGrayscale: error bmp or bmp->imagedata is null.");
    return false;
  }

  /*
  * ! performance wise :) */
  qword ubound = bmp->header.width * bmp->header.height;

  for(qword k=0;k<ubound;k++){
    color grsc = 0.299*bmp->imagedata[k].r + 0.587 * bmp->imagedata[k].g + 0.114 * bmp->imagedata[k].b;
    bmp->imagedata[k].r = bmp->imagedata[k].g = bmp->imagedata[k].b = grsc;
  }

  return true;
}
