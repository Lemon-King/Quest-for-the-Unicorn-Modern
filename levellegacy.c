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

void level_legacy(OBJECT_REC* item) {
  char flag;
  if (item == NULL) return;
  if (item->effect != E_LGCY) return;
  while (abs(item->level) < player.level) {
    item->level += (item->level > 0) ? 1 : -1;
    if (item->otype == O_DAGR) item->damage = item->control + (2 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_EDGE) item->damage = item->control + (3 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_POLE) item->damage = item->control + (3 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_BLNT) item->damage = item->control + (3 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_BOW) item->damage = item->control + (3 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_SHLD) item->damage = item->control + (1 * abs(item->level)) + (abs(item->level) / 5);
    if (item->otype == O_ARMR) item->damage = item->control + (3 * abs(item->level)) + (abs(item->level) / 2);
    if (item->otype == O_ROBE) item->damage = item->control + (1 * abs(item->level)) + (abs(item->level) / 5);
    if (item->otype == O_BOOT) item->damage = item->control + (1 * abs(item->level)) + (abs(item->level) / 5);
    if (item->otype == O_HELM) item->damage = item->control + (1 * abs(item->level)) + (abs(item->level) / 5);
    if (item->otype == O_DAGR) item->cost = (200 * abs(item->level));
    if (item->otype == O_EDGE) item->cost = (300 * abs(item->level));
    if (item->otype == O_POLE) item->cost = (300 * abs(item->level));
    if (item->otype == O_BLNT) item->cost = (300 * abs(item->level));
    if (item->otype == O_BOW) item->cost = (300 * abs(item->level));
    if (item->otype == O_SHLD) item->cost = (250 * abs(item->level));
    if (item->otype == O_ROBE) item->cost = (250 * abs(item->level));
    if (item->otype == O_BOOT) item->cost = (250 * abs(item->level));
    if (item->otype == O_HELM) item->cost = (250 * abs(item->level));
    if (item->otype == O_ARMR) item->cost = (350 * abs(item->level));
    if (abs(item->level) == 8) {
      item->num_enchantments = 1;
      item->enchantments[0] = mutable_enchant();
      }
    if (abs(item->level) == 10) {
      item->num_sockets = 1;
      flag = 0;
      while (flag == 0) {
        flag = -1;
        item->sockets[0] = mutable_enchant();
        if ((item->sockets[0] / 100) == (item->enchantments[0] / 100)) flag = 0;
        }
      }
    if (abs(item->level) == 15) {
      flag = 0;
      item->num_enchantments = 2;
      while (flag == 0) {
        flag = -1;
        item->enchantments[1] = mutable_enchant();
        if ((item->enchantments[1] / 100) == (item->enchantments[0] / 100)) flag = 0;
        if ((item->enchantments[1] / 100) == (item->sockets[0] / 100)) flag = 0;
        }
      }
    if (abs(item->level) == 17) {
      item->num_sockets = 2;
      flag = 0;
      while (flag == 0) {
        flag = -1;
        item->sockets[1] = mutable_enchant();
        if ((item->sockets[1] / 100) == (item->enchantments[0] / 100)) flag = 0;
        if ((item->sockets[1] / 100) == (item->enchantments[1] / 100)) flag = 0;
        if ((item->sockets[1] / 100) == (item->sockets[0] / 100)) flag = 0;
        }
      }
    if (abs(item->level) == 20) {
      item->num_enchantments = 3;
      flag = 0;
      while (flag == 0) {
        flag = -1;
        item->enchantments[2] = mutable_enchant();
        if ((item->enchantments[2] / 100) == (item->enchantments[0] / 100)) flag = 0;
        if ((item->enchantments[2] / 100) == (item->enchantments[1] / 100)) flag = 0;
        if ((item->enchantments[2] / 100) == (item->sockets[0] / 100)) flag = 0;
        if ((item->enchantments[2] / 100) == (item->sockets[1] / 100)) flag = 0;
        }
      }
    if (abs(item->level) == 22) {
      item->num_sockets = 3;
      flag = 0;
      while (flag == 0) {
        flag = -1;
        item->sockets[2] = mutable_enchant();
        if ((item->sockets[2] / 100) == (item->enchantments[0] / 100)) flag = 0;
        if ((item->sockets[2] / 100) == (item->enchantments[1] / 100)) flag = 0;
        if ((item->sockets[2] / 100) == (item->enchantments[2] / 100)) flag = 0;
        if ((item->sockets[2] / 100) == (item->sockets[0] / 100)) flag = 0;
        if ((item->sockets[2] / 100) == (item->sockets[1] / 100)) flag = 0;
        }
      }
    }
  }

