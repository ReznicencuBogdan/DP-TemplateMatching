/**
    Purpose: Defines the structures and algorithms used for geometric data sets.
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#ifndef GEOMETRICS_H
#define GEOMETRICS_H

#include "datatypes.h"

#pragma pack(push, 1)
typedef struct {
  dword x;
  dword y;
} point;

typedef struct {
  point lu;
  point rd;
} rect;
#pragma pack(pop)


/*
* @setPointCoordinates will update struct's calues according to parameters
* Note: It assumes that parameters are valid!  */
ntvoid setPointCoordinates(point *coord, dword x, dword y);

/*
* @setRectCoordinates sets rectangle points : top-left and bottom-right points */
ntvoid setRectCoordinates(rect *rc, dword x0, dword y0, dword x1, dword y1);

/*
* @getRectArea will return the area formed by the rectangle */
dword getRectArea(rect rc);

/*
* @rectsAreOverlapping tells if 2 rectangles are overlapping */
bool rectsAreOverlapping(rect r1, rect r2);

/*
* @getRectOverlapping returns the intersection of 2 rectangles as a rectangle */
rect getRectOverlapping(rect r1, rect r2);

/*
* @getRectOverlappingArea returns the area of the rectangle which acts as a intersection of 2 other rectangles */
dword getRectOverlappingArea(rect r1, rect r2);

/*
* @getRectOverlappingScore returns the score of intersection */
double getRectOverlappingScore(rect r1, rect r2);

#endif
