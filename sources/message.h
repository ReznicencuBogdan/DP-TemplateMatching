/**
    Purpose: Implements message (info/error) functions
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#ifndef MESSAGE_H
#define MESSAGE_H

#include "datatypes.h"


/*****************************************************************************
******************************************************************************
***********  Function definitions used for debug message manipulation  *******
******************************************************************************
*****************************************************************************/

/*
* @dispatchMessage will send stack trace data to the output and clears inner buffer */
ntvoid dispatchMessage();

/*
* @debugMessage inserts a new message into the internal buffer */
ntvoid debugMessage(const char *message);


#endif
