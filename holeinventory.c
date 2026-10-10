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


  void hole_show_inventory(Int32 base,OBJECT_REC* in_air) {
    Int32 i;
    char buffer[80];
    textmode(3);
    textcolor(15); gotoxy(29,1); txt_write("Portable Hole Inventory");
    sprintf(buffer,"     Page %d of 25",base/40+1);
    txt_write(buffer);
    gotoxy(10,3);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("ESC");
    textcolor(6); txt_write(">Exit, <");
    textcolor(14); txt_write("ENTER");
    textcolor(6); txt_write(">Transfer Item, <");
    textcolor(14); txt_write("Arrows");
    textcolor(6); txt_write(">Select Item, <");
    textcolor(14); txt_write("I");
    textcolor(6); txt_write(">nfo");
    gotoxy(11,4);
    textcolor(6);
    txt_write("<");
    textcolor(14); txt_write("7");
    textcolor(6); txt_write("> Exchange up, <");
    textcolor(14); txt_write("1");
    textcolor(6); txt_write(">Exchange down, <");
    textcolor(14); txt_write("space");
    textcolor(6); txt_write("> Exchange In Air");
    textcolor(3);
    for (i=0; i<20; i++) {
      if (player.hole[i+base].otype != O_NONE) {
        gotoxy(3,i+5);
        sprintf(buffer,"%s",get_name(&(player.hole[i+base])));
        if (strlen(buffer) > 30) {
          buffer[30] = 0;
          strcat(buffer,"...");
          }
        itemColor(&(player.hole[i+base]));
        txt_write(buffer);
        }
      if (player.hole[i+base+20].otype != O_NONE) {
        gotoxy(43,i+5);
        sprintf(buffer,"%s",get_name(&(player.hole[i+base+20])));
        if (strlen(buffer) > 30) {
          buffer[30] = 0;
          strcat(buffer,"...");
          }
        itemColor(&(player.hole[i+base+20]));
        txt_write(buffer);
        }
      }
    textcolor(CLR_WHITE);
    gotoxy(20,25); txt_write("In Air: ");
    if (in_air != NULL && in_air->otype != O_NONE) {
        sprintf(buffer,"%s",get_name(in_air));
        if (strlen(buffer) > 30) {
          buffer[30] = 0;
          strcat(buffer,"...");
          }
        itemColor(in_air);
        txt_write(buffer);
      }
    }

void hole_inventory() {
  char flag;
  char key;
  Int32  row;
  Int32  col;
  Int32  base;
  Int32  i;
  OBJECT_REC  it;
  OBJECT_REC* item;
  OBJECT_REC  in_air;
  OBJECT_REC  temp;
  Int32 item1;
  Int32 item2;

  row = 0;
  col = 0;
  base = 0;
  in_air.otype = O_NONE;
  hole_show_inventory(base,&in_air);
  flag = ' ';
  while (flag == ' ') {
    textcolor(14);
    gotoxy(1+col*40,row+5);
    txt_write("->");
    key = inkey();
    textcolor(14);
    gotoxy(1+col*40,row+5);
    txt_write("  ");
    if (key == 27) flag = 'N';
    if (key == 13) {
      it = player.hole[base+row+col*20];
      player.hole[base+row+col*20].otype = O_NONE;
      if (it.otype != O_NONE) item = inventory(&it);
        else item = inventory(NULL);
      if (item != NULL &&
        player.hole[base+row+col*20].otype == O_NONE) {
        player.hole[base+row+col*20] = *item;
        item->otype = O_NONE;
        }
      hole_show_inventory(base,&in_air);
      }
    if (key == 'i' || key == 'I') {
      it = player.hole[base+row+col*20];
      if (it.otype != O_NONE) data_card(&it);
      hole_show_inventory(base,&in_air);
      }
    if (key == '3') {
      base += 40;
      if (base > 960) base = 0;
      hole_show_inventory(base,&in_air);
      }
    if (key == '9') {
      base -= 40;
      if (base < 0) base = 960;
      hole_show_inventory(base,&in_air);
      }
    if (key == '2' || key == '.' || key == '>') {
      row++;
      if (row > 19) row = 0;
      }
    if (key == '8' || key == ',' || key == '<') {
      row--;
      if (row < 0) row = 19;
      }
    if (key == '6' || key == '4') {
      col = (col == 0) ? 1 : 0;
      }
    if (key ==' ' || key == 'x' || key == 'X') {
      item1 = base+row+col*20;
      temp = player.hole[item1];
      player.hole[item1] = in_air;
      in_air = temp;
      hole_show_inventory(base,&in_air);
      }
    if (key == '7') {
      item1 = row+col*20;
      item2 = item1 - 1;
      if (item2 < 0) item2 = 39;
      temp = player.hole[base + item1];
      player.hole[base + item1] = player.hole[base + item2];
      player.hole[base + item2] = temp;
      col = item2 / 20;
      row = item2 - (col * 20);
      hole_show_inventory(base,&in_air);
      }
    if (key == '1') {
      item1 = row+col*20;
      item2 = item1 + 1;
      if (item2 > 39) item2 = 0;
      temp = player.hole[base + item1];
      player.hole[base + item1] = player.hole[base + item2];
      player.hole[base + item2] = temp;
      col = item2 / 20;
      row = item2 - (col * 20);
      hole_show_inventory(base,&in_air);
      }
    }
  if (in_air.otype != O_NONE) {
    for (i=0; i<1000; i++)
      if (player.hole[i].otype == O_NONE) {
        player.hole[i] = in_air;
        i = 999;
        }
    }
  textmode(3);
  }

