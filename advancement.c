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

static void skills1(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<2; i++) {
    j = u_random(29);
    s = 0;
    if (j>=1 && j<=5) s=1;
    if (j>=6 && j<=10) s=2;
    if (j>=11 && j<=15) s=3;
    if (j==16) s=4;
    if (j==17) s=5;
    if (j==18) s=6;
    if (j==19) s=9;
    if (j==20) s=10;
    if (j==21) s=11;
    if (j==22) s=12;
    if (j==23) s=13;
    if (j==24) s=14;
    if (j==25) s=15;
    if (j>=26 && j<=28) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills2(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<3; i++) {
    j = u_random(42);
    s = 0;
    if (j>=1 && j<=5) s=1;
    if (j>=6 && j<=10) s=2;
    if (j>=11 && j<=15) s=3;
    if (j==16) s=4;
    if (j==17) s=5;
    if (j==18) s=6;
    if (j==19) s=7;
    if (j==20) s=8;
    if (j>=21 && j<=25) s=9;
    if (j>=26 && j<=30) s=10;
    if (j==31) s=11;
    if (j==32) s=12;
    if (j==33) s=13;
    if (j>=34 && j<=37) s=14;
    if (j==38) s=15;
    if (j>=39 && j<=41) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills3(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<3; i++) {
    j = u_random(42);
    s = 0;
    if (j>=1 && j<=5) s=1;
    if (j>=6 && j<=10) s=2;
    if (j>=11 && j<=15) s=3;
    if (j>=16 && j<=20) s=4;
    if (j>=21 && j<=25) s=5;
    if (j>=26 && j<=27) s=6;
    if (j==28) s=7;
    if (j==29) s=8;
    if (j==30) s=9;
    if (j==31) s=10;
    if (j>=32 && j<=34) s=11;
    if (j==35) s=12;
    if (j==36) s=13;
    if (j==37) s=14;
    if (j>=38 && j<=40) s=15;
    if (j==41) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills4(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<2; i++) {
    j = u_random(22);
    s = 0;
    if (j==1) s=1;
    if (j==2) s=2;
    if (j==3) s=3;
    if (j==4) s=4;
    if (j==5) s=5;
    if (j==6) s=6;
    if (j==7) s=7;
    if (j==8) s=8;
    if (j>=9 && j<=12) s=9;
    if (j>=13 && j<=15) s=10;
    if (j==16) s=11;
    if (j==17) s=12;
    if (j==18) s=13;
    if (j==19) s=14;
    if (j==20) s=15;
    if (j==21) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills5(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<3; i++) {
    j = u_random(30);
    s = 0;
    if (j==1) s=1;
    if (j==2) s=2;
    if (j==3) s=3;
    if (j>=4 && j<=6) s=4;
    if (j>=7 && j<=9) s=5;
    if (j>=10 && j<=11) s=6;
    if (j==12) s=7;
    if (j==13) s=8;
    if (j>=14 && j<=16) s=9;
    if (j>=17 && j<=19) s=10;
    if (j>=20 && j<=21) s=11;
    if (j>=22 && j<=23) s=12;
    if (j==24) s=13;
    if (j==25) s=14;
    if (j>=26 && j<=28) s=15;
    if (j==29) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills6(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<2; i++) {
    j = u_random(24);
    s = 0;
    if (j==1) s=1;
    if (j==2) s=2;
    if (j==3) s=3;
    if (j==4) s=4;
    if (j==5) s=5;
    if (j>=6 && j<=7) s=6;
    if (j>=8 && j<=10) s=7;
    if (j>=11 && j<=15) s=8;
    if (j==16) s=9;
    if (j==17) s=10;
    if (j==18) s=11;
    if (j==19) s=12;
    if (j==20) s=13;
    if (j==21) s=14;
    if (j==22) s=15;
    if (j==23) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

static void skills7(void) {
  Int32 i;
  Int32 j;
  Int32 s;
  return;
  for (i=0; i<2; i++) {
    j = u_random(41);
    s = 0;
    if (j>=1 && j<=5) s=1;
    if (j>=6 && j<=10) s=2;
    if (j>=11 && j<=15) s=3;
    if (j>=16 && j<=20) s=4;
    if (j>=21 && j<=25) s=5;
    if (j==26) s=6;
    if (j==27) s=9;
    if (j==28) s=10;
    if (j==29) s=11;
    if (j==30) s=12;
    if (j==31) s=13;
    if (j>=32 && j<=35) s=15;
    if (j>=36 && j<=40) s=16;
    if (s>0) {
      player.skill[s] += 100000;
      log_level_skill(s);
      }
    }
  }

/* The experience the next level needs, after a level needing exp_next */
double next_level_exp(double exp_next) {
  if (exp_next < 1e5) return exp_next + exp_next;

  return exp_next * classes[player.class-1].next_exp;
  }

void advancement() {
  Int32 i;

  log_level_up();
  player.level++;
  check_level_legacy();
  player_attack_defense();
  i = u_random(classes[player.class-1].hitdie) + 1;
  if (get_con() > 14) i += ((get_con() - 10) / 3);
  player.chp += i;
  player.mhp += i;
  log_hp_gain(i);
/*
  switch(classes[player.class-1].skill_table) {
    case 1:skills1(); break;
    case 2:skills2(); break;
    case 3:skills3(); break;
    case 4:skills4(); break;
    case 5:skills5(); break;
    case 6:skills6(); break;
    case 7:skills7(); break;
    }
*/
  advance_stat();
  adjust_abilities();
  if (classes[player.class-1].flags & CF_MONK) check_monk_weapon();
  if (classes[player.class-1].flags & CF_NOBLE) add_gold(player.level * 10000);
  if (player.mmana > 0) {
    i = u_random(classes[player.class-1].mana) + classes[player.class-1].mana;
    if (get_wis() > 15) i += ((get_wis() - 10) / 4);
    player.mmana += i;
    player.cmana += i;
    log_mana_gain(i);
/*    new_spell();  */
    show_screen();
    }
  if (player.mmana == 0 &&
      classes[player.class-1].becomes_caster> 0 &&
      player.level >= classes[player.class-1].becomes_caster) {
    i = u_random(classes[player.class-1].mana) + classes[player.class-1].mana;
    player.mmana += i;
    player.cmana += i;
    log_mana_gain(i);
/*    new_spell();  */
    show_screen();
    }
  player.exp_next = next_level_exp(player.exp_next);

  i = u_random(4);
  if (classes[player.class-1].level_skills[i] > 0) {
    player.skill[classes[player.class-1].level_skills[i]] += 100000;
    log_level_skill(classes[player.class-1].level_skills[i]);
    }
  set_title();
  }

