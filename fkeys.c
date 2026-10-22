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

static const Int32 FKEYS[12] = { KY_F1, KY_F2, KY_F3, KY_F4, KY_F5, KY_F6, KY_F7, KY_F8, KY_F9, KY_F10, KY_F11, KY_F12 };

/* 1 to 12 for F1 to F12; 0 for any other key */
Int32 fkey_number(Int32 key) {
  Int32 n;

  for (n=0; n<12; n++)
    if (key == FKEYS[n]) return n + 1;

  return 0;
  }

void fkey_pressed(Int32 n) {
  if (fkey_mode == 'S') cast_spell(n);
  if (fkey_mode == 'W') {
    if (n == 10) n = 0;
    load_gear('Y',n);
    }
  if (fkey_mode == 'G') {
    if (n == 10) n = 0;
    load_gear('N',n);
    }
  if (fkey_mode == 'M') strcpy(currentMacro, macros[n-1]);
  }

void shift_fkey_pressed(Int32 n) {
  if (fkey_mode == 'W') {
    if (n == 10) n = 0;
    save_gear('Y',n);
    }
  if (fkey_mode == 'G') {
    if (n == 10) n = 0;
    save_gear('N',n);
    }
  if (fkey_mode == 'S') {
    if (n == 10) n = 0;
    save_gear('Y',n);
    }
  if (fkey_mode == 'M') {
    if (n == 10) n = 0;
    save_gear('Y',n);
    }
  }

void alt_key_pressed(Int32 n) {
  if (fkey_mode == 'W' || fkey_mode == 'G') cast_spell(n);
  if (fkey_mode == 'S') {
    if (n == 10) n = 0;
    load_gear('Y',n);
    }
  if (fkey_mode == 'M') {
    if (n == 10) n = 0;
    load_gear('Y',n);
    }
  }

void ctrl_fkey_pressed(Int32 n) {
  if (fkey_mode != 'M') strcpy(currentMacro, macros[n-1]);
  if (fkey_mode == 'M') cast_spell(n);
  }

