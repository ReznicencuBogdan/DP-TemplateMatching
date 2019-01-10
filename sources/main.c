// ╔═══════════════════════════════════════════════════════════════╗
// ║ Content of encrypt.txt                                        ║
// ╠═══════════════════════════════════════════════════════════════╣
// ║ *first line path to image to encrypt                          ║
// ║ *second line path to where the encrypted image is to be saved ║
// ║ *third line path to secret_key.txt                            ║
// ╚═══════════════════════════════════════════════════════════════╝

// ╔═══════════════════════════════════════════════════════════════╗
// ║ Content of decrypt.txt                                        ║
// ╠═══════════════════════════════════════════════════════════════╣
// ║ *first line path to encrypted image                           ║
// ║ *second line path to where the decrypted image is to be saved ║
// ║ *third line path to secret_key.txt                            ║
// ╚═══════════════════════════════════════════════════════════════╝

// ╔══════════════════════════════════════════════════════════════════════════════╗
// ║ Content of decrypt.txt                                                       ║
// ╠══════════════════════════════════════════════════════════════════════════════╣
// ║ *first line path to image in which you want to find templates                ║
// ║ *next lines follow the following scheme:                                     ║
// ║     path_to_template, semnification(a string/char = meaning), red,green,blue ║
// ╚══════════════════════════════════════════════════════════════════════════════╝

// NOTE: VERY IMPORTANT !!! Avoid using paths which contain whitespaces!

// ╔═══════════════════════════════════════════════════════════════════════════════════════════════════╗
// ║ *   ADDENDUM: if you haven't read the README.txt file yet, and you do not know how to compile   * ║
// ║ *   this, then try READING it first.                                                              ║
// ╚═══════════════════════════════════════════════════════════════════════════════════════════════════╝


#include "message.h"
#include "bitmap.h"
#include "ocrdetect.h"

#include <string.h>
#include <stdlib.h>

ntvoid normalizePath(char *path) {
  path[strlen(path) - 1] = 0;
}

int main() {
  char image_path_source_1[MAX_PATH];
  char image_path_source_2[MAX_PATH];
  char image_path_source_3[MAX_PATH];
  char secret_key_path[MAX_PATH];
  char semnification[MAX_SEMNIFICATION_SIZE];

  ocrdata ocr;

  color red;
  color green;
  color blue;

  FILE *FIN;

  printf(C_YELLOW "Part 1. Configure the encrypt.txt file first.\n" C_RESET);
  FIN = fopen("encrypt.txt", "r");
  if(FIN) {
    fgets(image_path_source_1, MAX_PATH, FIN);
    normalizePath(image_path_source_1);

    // copy the path to original image so that it can be used for the chi square tests
    strcpy(image_path_source_3, image_path_source_1);

    fgets(image_path_source_2, MAX_PATH, FIN);
    normalizePath(image_path_source_2);

    fgets(secret_key_path, MAX_PATH, FIN);
    normalizePath(secret_key_path);

    fclose(FIN);

    if(!encryptBitmap(image_path_source_1,image_path_source_2,secret_key_path)){
      printf(C_RED "\tFailed encrypting image.\n" C_RESET);
      dispatchMessage();
    } else printf(C_YELLOW "\tFile encrypted.\n" C_RESET);
  }else printf(C_RED "\tError loading part 1.Skipping.\n" C_RESET);



  printf(C_YELLOW "Part 2. Configure the decrypt.txt file first.\n" C_RESET);
  FIN = fopen("decrypt.txt", "r");
  if(FIN) {
    fgets(image_path_source_1, MAX_PATH, FIN);
    normalizePath(image_path_source_1);

    fgets(image_path_source_2, MAX_PATH, FIN);
    normalizePath(image_path_source_2);

    fgets(secret_key_path, MAX_PATH, FIN);
    normalizePath(secret_key_path);

    fclose(FIN);

    if(!decryptBitmap(image_path_source_1,image_path_source_2,secret_key_path)){
      printf(C_RED "\tFailed decrypting image.\n" C_RESET);
      dispatchMessage();
    }else printf(C_YELLOW "\tFile decrypted.\n" C_RESET);
  }else printf(C_RED "\tError loading part 2.Skipping.\n" C_RESET);


  printf(C_YELLOW "Part 3. Showing Chi Square tests for original and encrypted images.\n" C_RESET);
  if(!chiSquareTest(image_path_source_3)) {
    printf(C_RED "\tFailed showing chisquare tests.\n" C_RESET);
    dispatchMessage();
  }
  if(!chiSquareTest(image_path_source_1)){
    printf(C_RED "\tFailed showing chisquare tests.\n" C_RESET);
    dispatchMessage();
  }



  printf(C_YELLOW "Part 4-5 Configure templates.txt first. Processing template matching.\n" C_RESET);
  FIN = fopen("templates.txt", "r");
  if(FIN) {
    fgets(image_path_source_1, MAX_PATH, FIN);
    normalizePath(image_path_source_1);

    if(initOcrTransaction(&ocr)) {
      addOcrInput(&ocr, image_path_source_1);
      addOcrOutput(&ocr, "MatchedTemplate.bmp");
      showOcrSemnificationsInConsole(&ocr, true);
      saveOcrDetectionsWithImage(&ocr, true);
      setOcrPrecision(&ocr, 2);

      printf(C_YELLOW "\t Collecting template data from templates.txt\n" C_RESET);
      while(fgets(image_path_source_1, MAX_PATH, FIN)) {
        normalizePath(image_path_source_1);

        char *pch = strtok (image_path_source_1 , " ,");
        strcpy(image_path_source_2, pch); // copy path

        pch = strtok (NULL, " ,");
        strncpy(semnification, pch, MAX_SEMNIFICATION_SIZE);

        pch = strtok (NULL, " ,");
        red = atoi(pch);

        pch = strtok (NULL, " ,");
        green = atoi(pch);

        pch = strtok (NULL, " ,");
        blue = atoi(pch);

        printf(C_YELLOW "\t Collected %s %s %u %u %u\n" C_RESET , image_path_source_2, semnification, red,green,blue);
        addOcrTrainingData(&ocr, image_path_source_2, semnification, rgb(red,green,blue));
      }

      printf(C_YELLOW "\t Beginning ocr transaction.\n" C_RESET);
      if(!beginOcrTransaction(&ocr)) {
        printf(C_RED "\t Failed matching templates in image.\n" C_RESET);
        dispatchMessage();
      }else
        printf("\nOpen now file MatchedTemplate.bmp(created in the path of execution)\n");

      disposeOcrTransaction(&ocr);
    }

    fclose(FIN);
  }else printf(C_RED "\tError loading part 4-5.Skipping.\n" C_RESET);

  printf(C_YELLOW "\nProgram ended.\n" C_RESET);

  return 0;
}
