/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include <stdio.h>

void debugLog(char* msg) {
  FILE* logFile;
  logFile = fopen("debug.log","a");
  fprintf(logFile,msg);
  fclose(logFile); 
  }

