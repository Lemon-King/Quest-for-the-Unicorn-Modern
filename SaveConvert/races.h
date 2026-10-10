/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

#define NUM_RACES   5

#ifdef MAIN

/*  Name          Str Dex Con Int Wis Chr    Skills         Skill levels    flags */
RACE_REC races[] = {
  { "Human",   {    1, -1,  0,  0,  0,  0 }, { 6, 7, 0, 0}, {-2,-2, 0, 0 }, 0, 50 },     // 0
  { "Elf",     {   -1, +1, -1,  0,  1,  1 }, { 1, 5,11, 0}, {-2,-2,-2, 0 }, 0, 400 },    // 1
  { "Half-Elf",{    0, +1,  0,  1,  1, -1 }, { 1, 0,11, 0}, {-2,-2,-2, 0 }, 0, 200 },    // 2
  { "Dwarf",   {    1,  1,  1, -1, -1, -1 }, {15,16, 0, 0}, {-2,-3, 0, 0 }, 0, 250 },    // 3
  { "Hobbit",  {   -1,  2,  1, -1, -1,  0 }, { 5,10, 0, 0}, {-3,-2, 0, 0 }, 0, 70 },     // 4
  };


TRAITS race_traits[] = {
  {   1,     1,    A_NOSL },
  {   1,     1,    A_NOFR },
  {   1,     1,    A_INFRA },
  {   2,     1,    A_INFRA },
  {   2,     1,    A_RSFR },
  {   2,     1,    A_RSSL },
  {   3,     1,    A_NOFR },
  {   3,     1,    A_NOPS },
  {   3,     1,    A_TRAP },
  {   3,    10,    A_SRCH },
  {   4,     1,    A_RSPS },
  {   4,     1,    A_RSFR },
  {   4,     1,    A_RSSL },
  {  99,    99,        99 },
  };

#else

extern RACE_REC races[];
extern TRAITS race_traits[];

#endif
