/**
    Purpose: Defines the algorithms used to detect templates.
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#ifndef OCR_DETECT_H
#define OCR_DETECT_H

#include "bitmap.h"
#include "vector.h"
#include "datatypes.h"


#define threshold double
#define minimum_detections 10


#define MAX_SEMNIFICATION_SIZE 10
#define MINIMUM_NUMBER_OF_TEMPLATES 10


typedef struct {
  char  image_path[MAX_PATH];
  char  semnification[MAX_SEMNIFICATION_SIZE];
  pixel frameColor;
} templatedata;

typedef struct {
  char    image_path[MAX_PATH];             // holds path of big image
  char    output_path[MAX_PATH];            // holds path of output image
  pvector templates;                        // array of templatedata
  bool    showSemnificationsInConsole;      // show on console the semnification of the template found in image
  bool    saveDetectionsWithImage;          // saves the image with detected frames on disk

  dword   precision;
  double  normxcorrDetectionThreshold;
  double  nonMaximumSupressionThreshold;
} ocrdata;


/************************************************************************************************************************
* THIS IS THE LIBRARY USED TO IDENTIFY DIGITS IN IMAGES. LIBRARY MAKES USE OF THE FOLLOWING PROCEDURES :
*     1) first thing to do is call initOcrTransaction (always)
*     2) after finishing work with the library use disposeOcrTransaction
*
************************************************************************************************************************/

/*
* @initOcrTransaction will initiate the sructure ocrdetect for furher use */
ntstatus initOcrTransaction(ocrdata *ocrd);

/*
* @addOcrInput initiates the big image in which templates are to be found */
ntstatus addOcrInput(ocrdata *ocrd, char *image_path);

/*
* @addOcrOutput asks for the path where the image(with colored found templates) is to be saved */
ntstatus addOcrOutput(ocrdata *ocrd, char *image_path);

/*
* @addOcrTrainingData add a template to find in some image. This function corelates a template with a semnification. */
ntstatus addOcrTrainingData(ocrdata *ocrd, char *image_path, char *semnification, pixel frameColor);

/*
* @showOcrSemnificationsInConsole will show in the console the semnification of the templates found in the image.
* @note: at the moment this function doesn't print the semnifications in the order found in the image originally */
ntstatus showOcrSemnificationsInConsole(ocrdata *ocrd, bool flag);

/*
* @saveOcrDetectionsWithImage enables/disables the process of saving the image with identified templates framed in colred rectangles */
ntstatus saveOcrDetectionsWithImage(ocrdata *ocrd, bool flag);

/*
* @setOcrDetectionThreshold sets the threshold */
ntstatus setOcrDetectionThreshold(ocrdata *ocrd, double thrsh);

/*
* @setOcrSupressionThreshold sets the threshold */
ntstatus setOcrSupressionThreshold(ocrdata *ocrd, double thrsh);

/*
* @setOcrPrecision affects performance of the algoritm and effciency */
ntstatus setOcrPrecision(ocrdata *ocrd, dword precision);

/*
* @beginOcrTransaction will start the process of findind the templates in the image */
ntstatus beginOcrTransaction(ocrdata *ocrd);

/*
* @disposeOcrTransaction clears the resources alocated by this process of template matching */
ntstatus disposeOcrTransaction(ocrdata *ocrd);

/*
* @normxcorr handles the process of matchTemplate */
ntstatus normxcorr(ocrdata *ocrd);

/*
* @matchTemplate is the actual process of finding a template. The positions of the rectangles are appended to the vector of detections.
* @note: data regardind the template to be matched are passed through the tdata parameter. */
ntstatus matchTemplate(bitmap *image, bitmap *temp, templatedata *tdata, pvector vectorOfDetections, threshold thrsh, dword precision);

#endif
