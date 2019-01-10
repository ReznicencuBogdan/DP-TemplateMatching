/*
    Purpose: Implements the ocr detection algorithms
    @version 1.0.0 1/1/2019
    @code may not be used without the consent of the author
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ocrdetect.h"
#include "message.h"

#define DETECTION_SENSIBILITY 0.5f
#define SUPRESSION_SENSIBILITY 0.1f

typedef struct  {
  templatedata *tdata;          // holds a refference to one element in the vector of templates.(this could be a terrible idea when using multithreading.)
  double        correlation;    // holds detetction score
  rect          frame;          // holds the bounds of detection
  qword         uid;            // identifies this structure as a unique entitiy (progressive id)
} detectionData;



/****************************************************************************************************
* --------------- FUNCTIONS RELATED TO OCR SETUP ---------------------------------------------------
****************************************************************************************************/

ntstatus initOcrTransaction(ocrdata *ocrd) {
  if(ocrd == NULL) {                                                          // do some parameter checking
    debugMessage("initOcrTransaction: invalid parameter.");
    return false;
  }

  // clear the structure so that there won't be trashy data
  memset( (void*)ocrd, 0x00, sizeof(ocrdata) );

  // set precision to a default value
  ocrd->precision = 2;

  // setting default thresholds
  ocrd->normxcorrDetectionThreshold = DETECTION_SENSIBILITY;
  ocrd->nonMaximumSupressionThreshold = SUPRESSION_SENSIBILITY;

  // create a vector which will hold info about the templates
  ocrd->templates = vector_create(MINIMUM_NUMBER_OF_TEMPLATES, sizeof(templatedata));

  // check the integrity of the vector
  if(ocrd->templates == NULL) {
    debugMessage("addOcrInput: failed to allocate enough memory.");
    return false;
  }

  // note: do not forget to dispose the vector
  return true;
}

ntstatus addOcrInput(ocrdata *ocrd, char *image_path) {
  // do some parameter check
  if(ocrd == NULL || image_path == NULL) {
    debugMessage("addOcrInput: invalid parameters.");
    return false;
  }

  // copy the path into the structures array
  strncpy(ocrd->image_path, image_path, MAX_PATH - 1);
  ocrd->image_path[MAX_PATH - 1] = 0;

  return true;
}

ntstatus addOcrOutput(ocrdata *ocrd, char *image_path) {
  // do some parameter check
  if(ocrd == NULL || image_path == NULL) {
    debugMessage("addOcrOutput: invalid parameters.");
    return false;
  }

  // copy the path into the structures array
  strncpy(ocrd->output_path, image_path, MAX_PATH - 1);
  ocrd->output_path[MAX_PATH - 1] = 0;

  return true;
}

ntstatus addOcrTrainingData(ocrdata *ocrd, char *image_path, char *semnification, pixel frameColor) {
  if(ocrd == NULL || image_path == NULL) {
    debugMessage("addOcrTrainingData: invalid parameters.");
    return false;
  }

  if(ocrd->templates == NULL) {
    debugMessage("addOcrTrainingData: vector uninitialized.");
    return false;
  }


  // create a structure which holds data about the template
  // the structure will be pushed into a vector
  templatedata temporaryTemplate;


  // save the path into the structure's vector and make sure that there are no overflows
  strncpy(temporaryTemplate.image_path, image_path, MAX_PATH - 1);
  temporaryTemplate.image_path[MAX_PATH - 1] = 0;


  // save the string 'semnification' into the struct and make sure there are no overflows
  strncpy(temporaryTemplate.semnification, semnification, MAX_SEMNIFICATION_SIZE - 1);
  temporaryTemplate.semnification[MAX_SEMNIFICATION_SIZE - 1] = 0;


  // save the frame color into the struct
  temporaryTemplate.frameColor = frameColor;


  // push the struct into the vector
  vector_push_back(ocrd->templates, (pvoid)&temporaryTemplate);

  return true;
}

ntstatus showOcrSemnificationsInConsole(ocrdata *ocrd, bool flag) {
  ocrd->showSemnificationsInConsole = flag;
  return true;
}

ntstatus saveOcrDetectionsWithImage(ocrdata *ocrd, bool flag) {
  ocrd->saveDetectionsWithImage = flag;
  return true;
}

ntstatus setOcrDetectionThreshold(ocrdata *ocrd ,double thrsh) {
  ocrd->normxcorrDetectionThreshold = thrsh;
  return true;
}

ntstatus setOcrSupressionThreshold(ocrdata *ocrd, double thrsh) {
  ocrd->nonMaximumSupressionThreshold = thrsh;
  return true;
}

ntstatus setOcrPrecision(ocrdata *ocrd, dword precision) {
    ocrd->precision = precision;
    return true;
}

ntstatus setOcrSaveDetectionsWithImage(ocrdata *ocrd, bool flag) {
  ocrd->saveDetectionsWithImage = flag;
  return true;
}

ntstatus beginOcrTransaction(ocrdata *ocrd) {
  if(ocrd == NULL) {
    debugMessage("beginOcrTransaction: invalid parameter.");
    return false;
  }

  if(ocrd->templates == NULL) {
    debugMessage("beginOcrTransaction: vector of templates is uninitialized.");
    return false;
  }

  return normxcorr(ocrd);
}

ntstatus disposeOcrTransaction(ocrdata *ocrd) {
  if(ocrd == NULL) {
    debugMessage("disposeOcrTransaction: invalid parameter.");
    return false;
  }

  if(ocrd->templates == NULL) {
    debugMessage("disposeOcrTransaction: vector uninitialized.");
    return false;
  }

  // clear aloocated space for the vector
  vector_release(ocrd->templates);

  return true;
}


/****************************************************************************************************
* --------------- FUNCTIONS RELATED STRICTLY TO DETECTION OF TEMPLATES ------------------------------
****************************************************************************************************/

int functorCallbackSortTemplates(const pvoid p1, const pvoid p2) {
  double c1 = (*(detectionData*)p1).correlation;
  double c2 = (*(detectionData*)p2).correlation;

  if(c1 > c2) return -1;

  return c1 < c2;
}

ntvoid sortTemplateDetections(pvector vec) {
  qsort(vector_begin(vec), vector_count(vec), sizeof(detectionData), functorCallbackSortTemplates);
}

ntvoid supressNonMaximums(pvector detectionVector, threshold thrsh) {
  // holds overlapping score
  rect di;
  rect dj;

  for(size_t i = 0;i < vector_count(detectionVector); i++)
  for(size_t j = i+1;j < vector_count(detectionVector); j++) {
    di = (*(detectionData*)vector_at(detectionVector, i)).frame;
    dj = (*(detectionData*)vector_at(detectionVector, j)).frame;

    if(rectsAreOverlapping(di,dj)) {
      double score = getRectOverlappingScore(di, dj);

      if(score > thrsh)
      vector_erase(detectionVector, j);
    }
  }
}

ntstatus normxcorr(ocrdata *ocrd) {
  ntstatus processResult;
  pvector detectionVector;
  bitmap template;
  bitmap image;

  // by default this process is succesfull
  processResult = true;

  if(ocrd == NULL) {
    debugMessage("normxcorr: invalid ocr data given to the preprocessor.");
    return false;
  }

  if(ocrd->templates == NULL) {
    debugMessage("normxcorr: invalid ocr data given.Uninitialized template vector space.");
    return false;
  }

  // create a vector which stores all detections
  detectionVector = vector_create(minimum_detections, sizeof(detectionData));

  // maybe there is no more memory available
  if(detectionVector == NULL)  {
    debugMessage("normxcorr: error detectionVector is null.");
    return false;
  }

  // read the main image to do cross correlation on
  if(!readBitmap(ocrd->image_path, &image)) {
    debugMessage("normxcorr: failed reading image.");
    vector_release(detectionVector);
    return false;
  }

  if(!cvrtBitmapGrayscale(&image)) {
    debugMessage("normxcorr: failed converting image to grayscale.");
    vector_release(detectionVector);
    freeBitmap(&image);
    return false;
  }


  // readding templates and perforimng cross corelation on each of those
  for(size_t i=0;i<vector_count(ocrd->templates);i++) {
    templatedata *tdata = (templatedata*)vector_at(ocrd->templates, i);           // holds data for this selected template

    if(!readBitmap(tdata->image_path , &template)) {
      debugMessage("normxcorr: failed reading template.Skipping this template.");

      processResult = false;
      continue;
    }

    if(!cvrtBitmapGrayscale(&template)) {                                         // trying to convert image to grayscale
      debugMessage("normxcorr: failed converting template to grayscale.Skipping this template.");
      freeBitmap(&template);

      processResult = false;
      continue;
    }

    // loaded all necesary resources until now therefore perform normalized-cross-correlation
    matchTemplate(&image, &template,tdata, detectionVector, ocrd->normxcorrDetectionThreshold, ocrd->precision);

    // release resources handled by the template
    freeBitmap(&template);
  }


  // sort detections
  sortTemplateDetections(detectionVector);

  // eliminate non-maximum
  supressNonMaximums(detectionVector, ocrd->nonMaximumSupressionThreshold);

  // prompt user with some nice message
  if(ocrd->showSemnificationsInConsole)
  printf(C_YELLOW "\nFound the next area of semnifications in image:\n" C_RESET);

  // show detected rectangles in image only if I want to
  for (pvoid i =  vector_begin(detectionVector);
             i != vector_end(detectionVector);
             i =  vector_next(detectionVector, i)) {

    // break the loop if there is no flag set.
    if(ocrd->saveDetectionsWithImage == false && ocrd->showSemnificationsInConsole == false)
      break;

    detectionData *dData = (detectionData*)i;


    if(ocrd->saveDetectionsWithImage == true)
      drawBitmapRect(&image,dData->frame , dData->tdata->frameColor);    // show the frames on the original image

    if(ocrd->showSemnificationsInConsole)
      printf("%s ", dData->tdata->semnification);                     // show then the semnification of each idenitified template in the console
  }


  if(ocrd->saveDetectionsWithImage == true) {   // writting the image with the recongized patterns on disk only if the user wants to
    if(ocrd->output_path[0] != 0)               // save the image with detections framed to output path
      writeBitmap(ocrd->output_path, &image);   // if the path has been initialized
    else {                                      // if the path wasn't specified but the user wanted to save it.
      debugMessage("normxcorr: output path wasn't specified.");
      processResult = false;
    }
  }

  // release resources
  vector_release(detectionVector);
  freeBitmap(&image);

  return processResult;
}


// templatedata can be NULL. it only holds additional data regarding the template given so that it can be more easily shown to a human.
ntstatus matchTemplate(bitmap *image, bitmap *temp,templatedata *tdata, pvector vectorOfDetections, threshold thrsh, dword precision) {
  if(image == NULL || temp == NULL) return false;
  if(image->imagedata == NULL || temp->imagedata == NULL) return false;

  dword frameWidth, frameHeight,halfFrameWidth, halfFrameHeight, imageWidth, imageHeight;
  detectionData detectedframe;
  qword templateSize;
  point coord, coord1;

  frameWidth = temp->header.width;
  frameHeight = temp->header.height;
  halfFrameWidth = frameWidth / 2;
  halfFrameHeight = frameHeight / 2;
  imageWidth = image->header.width;
  imageHeight = image->header.height;
  templateSize = frameWidth * frameHeight;

  double correlation;
  double averageImage = 0;
  double averageTemplate = 0;

  double stdImage=0;
  double stdTemplate=0;

  double sqrdpxl_template = 0;
  double sqrdpxl_image = 0;

  dword i,ii;
  dword j,jj;

  // manually calculate the average of the template window
  for(ii=0;ii<frameWidth;ii++)
  for(jj=0;jj<frameHeight;jj++){
        setPointCoordinates(&coord, ii, jj);
        averageTemplate += getBitmapPixelAt(temp, coord).r;
        sqrdpxl_template += pow( getBitmapPixelAt(temp, coord).r, 2);
  }


  // calculate standard deviation and averages
  stdTemplate = averageTemplate;
  averageTemplate = averageTemplate / templateSize;
  stdTemplate = sqrt(
                      (sqrdpxl_template - 2*averageTemplate * stdTemplate + templateSize * pow(averageTemplate, 2))
                      /
                      (templateSize - 1)
               );   // looks more like matlab


  // dynamically calculate corelation and related data(average, standard deviation etc)
  for(i = halfFrameWidth; i < imageWidth - halfFrameWidth; i += precision) {
    for(j = halfFrameHeight ; j < imageHeight - halfFrameHeight; j += precision) {
      // reset variables for calculating the corelation for the next set of pixels
      averageImage = 0;
      stdImage = 0;
      correlation = 0;
      sqrdpxl_image = 0;

      // manually calculate the average of the image window
      for(ii=i-halfFrameWidth;ii<i+halfFrameWidth;ii++)
      for(jj=j-halfFrameHeight;jj<j+halfFrameHeight;jj++){
            setPointCoordinates(&coord, ii, jj);
            averageImage += getBitmapPixelAt(image, coord).r;
            sqrdpxl_image += pow( getBitmapPixelAt(image, coord).r, 2);
      }

      stdImage = averageImage;
      averageImage = averageImage / templateSize;
      stdImage = sqrt(
                      (sqrdpxl_image - 2*averageImage * stdImage + templateSize * pow(averageImage, 2))
                      /
                      (templateSize - 1)
                );

      for(ii=i-halfFrameWidth;ii<i+halfFrameWidth;ii++)
      for(jj=j-halfFrameHeight;jj<j+halfFrameHeight;jj++){
            setPointCoordinates(&coord, ii, jj);
            setPointCoordinates(&coord1, ii - i + halfFrameWidth, jj - j + halfFrameHeight);
            correlation += (getBitmapPixelAt(image, coord).r - averageImage) * (getBitmapPixelAt(temp, coord1).r - averageTemplate)
                           /
                           (stdImage * stdTemplate);
      }

      correlation = correlation / templateSize;

      if(correlation >= thrsh) {
        detectedframe.correlation = correlation;
        detectedframe.tdata = tdata;

        setRectCoordinates(&detectedframe.frame, i-halfFrameWidth, j-halfFrameHeight, i+halfFrameWidth, j+halfFrameHeight);
        vector_push_back(vectorOfDetections, (pvoid)&detectedframe);
      }
    }
  }

  return true;
}
