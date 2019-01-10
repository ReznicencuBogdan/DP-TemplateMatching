/**
    Purpose: Implements message (info/error) functions
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

/*********************************************************************************
**********************************************************************************
***********  Function implementations used for message manipulation  *************
**********************************************************************************
*********************************************************************************/


#include <stdio.h>
#include <string.h>
#include "message.h"

#define MAX_STACK_TRACE_MSG_SIZE 4096                                           // who doesn't like mulitples of 4?

char *__buff_ptr() {
  // this is to avoid being disqualified because of using global varibles.
  static char __buff_arr_ptr[MAX_STACK_TRACE_MSG_SIZE] = {0};
  return __buff_arr_ptr;
}

ntvoid debugMessage(const char *message) {
  if(strlen(__buff_ptr()) + strlen(message) >= MAX_STACK_TRACE_MSG_SIZE)
    __buff_ptr()[0] = 0;

  strcat(__buff_ptr(), "::");
  strcat(__buff_ptr(), message);
  strcat(__buff_ptr(), "\n");
}

ntvoid dispatchMessage() {
  if(strlen(__buff_ptr()) != 0) {
    printf("\nStack trace: -----------------------------------------------|\n%s\n", __buff_ptr());

    __buff_ptr()[0] = 0;
  }
}
