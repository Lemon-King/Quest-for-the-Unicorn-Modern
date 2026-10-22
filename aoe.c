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

/* ******************************************************************* */
/* ***** Generalized area of effect to hit and damage processing ***** */
/* ***** cx - x position of center of damage                     ***** */
/* ***** cy - y position of center of damage                     ***** */
/* ***** effect - E_effect to perform on hit                     ***** */
/* ***** control - Control value for effect                      ***** */
/* ***** damage  - Amount of damage/time/etc.                    ***** */
/* *****           -amount = non-random amount                   ***** */
/* *****           +amount = random amount                       ***** */
/* ***** element - Element of effect                             ***** */
/* ***** range   - Range of effect, 0 if single target           ***** */
/* ***** mtype   - Type of attack M_type                         ***** */
/* ***** tohit   - Value needed to hit                           ***** */
/* *****           -amount = specific percentage needed to hit   ***** */
/* *****           +amount = Monster level effects to hit        ***** */
/* ******************************************************************* */

void aoe(Int32 cx,Int32 cy,Int32 effect,Int32 control,Int32 damage,Int32 element,Int32 range,Int32 mtype,Int32 toHit) {
  NPC_REC* monster;
  Int32 dmg;
  Int32 x,y;
  Int32 tmp;
  Int32 needed;
  char crit;
  for (y=cy-range; y<=cy+range; y++)
    for (x=cx-range; x<=cx+range; x++) {
      monster = get_monster(x,y);
      if (monster != NULL) {
        if (toHit > 0) {
          needed = toHit - (monster->level * 10);
          if (needed < 10) needed = 10;
          if (needed > 90) needed = 90;
          } else needed = 0-toHit;
        if (mtype == M_MAGIC && monster != NULL && (monster->special & MS_AMAGIC) == MS_AMAGIC) needed = 0;
        if (mtype == M_PHYSICAL && monster != NULL && (monster->special & MS_APHYSICAL) == MS_APHYSICAL) needed = 0;
        if (monster != NULL && u_random(100) < needed) {
          if ((monster->special & MS_FRIENDLY) == MS_FRIENDLY) {
            player.align -= 5;
            if (player.align < 0) player.align = 0;
            }
          dmg = (damage <= 0) ? -damage : (damage / 2) + u_random(damage/2) + 1;
          /* *********************** */
          /* ***** Heal effect ***** */
          /* *********************** */
          if (effect == E_HEAL) {
            monster->hp += dmg;
            log_creature_healed(monster,dmg);
            }

          /* ************************* */
          /* ***** Damage effect ***** */
          /* ************************* */
          if (effect == E_DAMG) {
            crit = 0;
            if (u_random(100) < get_crit()) {
              dmg *= 2;
              stats.counts[STAT_CRITS]++;
              crit = -1;
              }
            if (in_sight(x,y) == '*') {
              show_sprite(x-player.map_x,y-player.map_y,100);
              delay(delay_hit);
              show_spot(x,y);
              }
            tmp = figure_damage(dmg,element,monster);
            if (monster->defense > 95) {
              tmp -= (Int32)(95.0 / 100.0 * tmp);
              } else {
              tmp -= (Int32)(monster->defense / 100.0 * tmp);
              }
            log_hit(monster,tmp,crit);
            stats_do_damage(tmp);
            monster->hp -= tmp;
            dead_monster(monster,'Y',x,y);
            if (mtype == M_MAGIC) add_skill(SK_MAGIC,1);
            }

          /* ************************ */
          /* ***** Sleep effect ***** */
          /* ************************ */
          if (effect == E_SPCL && control == 60) {
            monster->sleep = dmg;
            log_creature_asleep(monster);
            }

          /* *********************** */
          /* ***** Fear effect ***** */
          /* *********************** */
          if (effect == E_SPCL && control == 61) {
            monster->fear = dmg;
            log_creature_afraid(monster);
            }

          /* ************************ */
          /* ***** Blind effect ***** */
          /* ************************ */
          if (effect == E_SPCL && control == 62) {
            monster->blind = dmg;
            log_creature_blinded(monster);
            }

          /* ***************************** */
          /* ***** Confustion effect ***** */
          /* ***************************** */
          if (effect == E_SPCL && control == 63) {
            monster->confused = dmg;
            log_creature_confused(monster);
            }

          /* *********************************** */
          /* ***** Clear Confustion effect ***** */
          /* *********************************** */
          if (effect == E_SPCL && control == 64) {
            monster->confused = 0;
            }

          /* ***************************** */
          /* ***** Clear Fear effect ***** */
          /* ***************************** */
          if (effect == E_SPCL && control == 65) {
            monster->fear = 0;
            }

          /* ********************************** */
          /* ***** Clear Blindness effect ***** */
          /* ********************************** */
          if (effect == E_SPCL && control == 66) {
            monster->blind = 0;
            }

          /* **************************** */
          /* ***** Clear All effect ***** */
          /* **************************** */
          if (effect == E_SPCL && control == 67) {
            monster->blind = 0;
            monster->fear = 0;
            monster->confused = 0;
            monster->poison = 0;
            }

          /* ************************* */
          /* ***** Poison effect ***** */
          /* ************************* */
          if (effect == E_POSN) {
            monster->poison = dmg;
            log_creature_poisoned(monster);
            }
        }
      else log_miss(monster,(needed == 0) ? RESISTED : MISSED);
      }
    }


  }

