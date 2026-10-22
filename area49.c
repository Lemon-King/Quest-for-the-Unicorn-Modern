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

/* Without the Blast Spells change special is read as an old number, not the MS_ bits */
static Int32 is_anti_magic(NPC_REC* target) {
  if (change_blast_spells == 'Y') return ((target->special & MS_AMAGIC) == MS_AMAGIC);
  return (target->special == 1);
  }

static Int32 is_friendly(NPC_REC* target) {
  if (change_blast_spells == 'Y') return ((target->special & MS_FRIENDLY) == MS_FRIENDLY);
  return (target->special > 9);
  }

static void make_hostile(NPC_REC* target) {
  if (change_blast_spells == 'Y') target->special -= MS_FRIENDLY;
    else target->special -= 10;
  }

void area49(Int32 damage,Int32 elem,Int32 center_x,Int32 center_y) {
  Int32  x,y;
  Int32  x1,y1;
  Int32  mon;
  Int32  i;
  Int32  crit;
  x1 = center_x; y1 = center_y;
  if (x1<3) x1=3; else if (x1>97) x1 = 97;
  if (y1<3) y1=3; else if (y1>97) y1 = 97;
  for (x=x1-3; x<=x1+3; x++)
    for (y=y1-3; y<=y1+3; y++)
      if (player.map[x][y] < 0 || (player.map[x][y]>400 && player.map[x][y]<500)) {
        if (in_arena == 'Y') mon = player.map[x][y] - 400;
          else mon = player.map[x][y];
        if (mon < 0) mon = -mon;
        i = 90;
        if (in_arena == 'Y' && is_anti_magic(&player.monster[mon][a_mon])) i = 0;
        if (in_arena != 'Y' && is_anti_magic(&player.monster[1][mon])) i = 0;
        if (u_random(100) < i) {
          clear_prompt();
          noise(1000,100);
          log_hit_shout();
          if (in_arena != 'Y' && is_friendly(&player.monster[1][mon])) {
            player.align -= 20;
            make_hostile(&player.monster[1][mon]);
            if (player.align < 0) player.align = 0;
            }
          if (in_arena == 'Y' && is_friendly(&player.monster[mon][a_mon])) {
            player.align -= 20;
            make_hostile(&player.monster[mon][a_mon]);
            if (player.align < 0) player.align = 0;
            }
          if (in_sight(x,y) == '*') {
            show_sprite(x-player.map_x,y-player.map_y,80);
            delay(250);
            show_spot(x,y);
            }
          i = u_random(damage) + 1;
          crit = HIT_NORMAL;
          if (u_random(100) < get_crit()) {
            i *= 2;
            stats.counts[STAT_CRITS]++;
            crit = HIT_CRITICAL;
            }
          if (in_arena == 'Y') i = figure_damage(i,elem,&(player.monster[mon][a_mon]));
            else  i = figure_damage(i,elem,&(player.monster[1][mon]));
          stats_do_damage(i);
          log_spell_hit(monster_record(mon),i,crit);
          if (in_arena == 'Y') player.monster[mon][a_mon].hp -= i;
            else player.monster[1][mon].hp -= i;
          if (in_arena == 'Y') i=player.monster[mon][a_mon].hp;
            else i=player.monster[1][mon].hp;
          if (i <= 0) {
            clear_prompt();
            if (in_arena != 'Y') spell_kill(&(player.monster[1][mon]),x,y);
            if (in_arena == 'Y') spell_kill(&(player.monster[mon][a_mon]),x,y);
            show_spot(x,y);
            if (in_arena != 'Y' && is_friendly(&player.monster[1][mon])) {
              player.align -= 20;
              if (player.align < 0) player.align = 0;
              }
            if (in_arena == 'Y' && is_friendly(&player.monster[mon][a_mon])) {
              player.align -= 20;
              if (player.align < 0) player.align = 0;
              }
            show_side();
            }

          }
        else {
          clear_prompt();
          log_miss(monster_record(mon),(i == 0) ? RESISTED : MISSED);
          stats.counts[STAT_MISSES]++;
          }
        }
  }
