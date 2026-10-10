/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#include "header.h"
#include "gothic.h"

/* The BGI Gothic font (Copyright (c) 1987 Borland International), built on first use. */
STROKE_FONT* gothic_font() {
  static STROKE_FONT* font = NULL;
  if (font == NULL) font = stroke_font_create(gothic_chr,sizeof(gothic_chr));
  return font;
  }
