/*
    Purpose: Implements the algorithms used on geometric data.
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "geometrics.h"

dword max(dword x, dword y) {
  return (x > y ? x : y);
}

dword min(dword x, dword y) {
  return ( x < y ? x : y);
}

ntvoid setPointCoordinates(point *coord, dword x, dword y) {
  coord->x=x;
  coord->y=y;
}


ntvoid setRectCoordinates(rect *rc, dword x0, dword y0, dword x1, dword y1){
  rc->lu.x=x0;
  rc->lu.y=y0;
  rc->rd.x=x1;
  rc->rd.y=y1;
}

dword getRectArea(rect rc) {
  dword area = (rc.rd.x - rc.lu.x) * (rc.rd.y - rc.lu.y);
  return area;
}


bool rectsAreOverlapping(rect r1, rect r2) {
  return  !(r1.lu.x > r2.rd.x || r2.lu.x > r1.rd.x || r1.lu.y > r2.rd.y || r2.lu.y > r1.rd.y);
}


rect getRectOverlapping(rect r1, rect r2) {
  // NOTE: before using this function one must first check if the rectangles are overlapping
  rect rc;
  setRectCoordinates(&rc, max( r1.lu.x, r2.lu.x ),
                          max( r1.lu.y, r2.lu.y ),
                          min( r1.rd.x, r2.rd.x ),
                          min( r1.rd.y, r2.rd.y));
  return rc;
}


dword getRectOverlappingArea(rect r1, rect r2) {
  if(!rectsAreOverlapping(r1,r2)) return 0;

  return getRectArea(getRectOverlapping(r1,r2));
}


double getRectOverlappingScore(rect r1, rect r2) {
  double overlappingArea = getRectOverlappingArea(r1,r2);
  return overlappingArea / (getRectArea(r1) + getRectArea(r2) - overlappingArea);
}
