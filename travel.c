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

#define SHIP_LABEL "Purchase a ship"

/* The trips offered: dest[1] to dest[count] at their costs, then a ship when the town has a harbour */
typedef struct {
  char   (*dest)[33];
  Int32* cost;
  Int32  count;
  } TRIPS;

static void draw_trip(MENU* menu,Int32 row,Int32 line) {
  TRIPS* trips;
  char   price[16];

  trips = menu->screen;
  if (row < trips->count) {
    sprintf(price,"%4d",trips->cost[row + 1]);
    draw_menu_text(menu,line,trips->dest[row + 1],price);
    }
  else draw_menu_text(menu,line,SHIP_LABEL,"2500");
  }

static void new_town(Int32 where) {
  Int32 i;
//    Int32 m_file;
//#ifdef MINGW
//    m_file = open("temp01.$$$",O_WRONLY | O_BINARY);
//#else
//    m_file = open("temp01.$$$",O_WRONLY);
//#endif
//    if (m_file < 0) {
//      printf("Fatal Error: Could not open temp01.$$$\n");
//      exit(1);
//      }
//    lseek(m_file,sizeof(KNOW_TYPE) * player.city,SEEK_SET);
//    write(m_file,&(player.known),sizeof(KNOW_TYPE));
//    close(m_file);
  memcpy(&(known_maps[player.city]), &(player.known), sizeof(KNOW_TYPE));

  player.region = links[where].region;
  player.city = links[where].diff;
  player.land_x = links[where].x;
  player.land_y = links[where].y;
  player.x = links[where].sx;
  player.y = links[where].sy;
  strcpy(player.d_name,links[where].name);

  /* A town reached by ship is entered too (see enter_dungeon() in upland.c) */
  if (change_town_completion == 'Y') player.quests[where] = 'Y';
  copyMap(&player.map,player.city);
  place_town_ship();
//#ifdef MINGW
//    m_file = open("temp01.$$$",O_RDONLY | O_BINARY);
//#else
//    m_file = open("temp01.$$$",O_RDONLY);
//#endif
//    if (m_file < 0) {
//      printf("Fatal Error: Could not open temp01.$$$\n");
//      exit(1);
//      }
//    lseek(m_file,sizeof(KNOW_TYPE) * player.city,SEEK_SET);
//    read(m_file,&(player.known),sizeof(KNOW_TYPE));
//    close(m_file);
  memcpy(&(player.known), &(known_maps[player.city]), sizeof(KNOW_TYPE));
  i = 0;
  while (i >= 0) {
    if (shops[i].shoptype != 999) {
      if (shops[i].region == player.region &&
          shops[i].rx == player.land_x &&
          shops[i].ry == player.land_y)
        player.map[shops[i].cx][shops[i].cy] = 520;
      i++;
      } else i = -1;
    }

  stock_town(player.city);
  player.map_x = player.x-TILES_X/2;
  player.map_y = player.y-TILES_Y/2;
  if (player.map_x<1) player.map_x = 1;
  if (player.map_y<1) player.map_y = 1;
  if (player.map_x>100-TILES_X) player.map_x = 100-TILES_X;
  if (player.map_y>100-TILES_Y) player.map_y = 100-TILES_Y;
  camera_arrive();
  i = 0;
  while (i >= 0) {
    if (forsale[i].region != 999) {
      if (forsale[i].region == player.region &&
          forsale[i].rx == player.land_x &&
          forsale[i].ry == player.land_y)
        player.map[forsale[i].cx][forsale[i].cy] = S_SALE;
      i++;
      } else i = -1;
    }
  for (i=0; i<5; i++)
    if (player.houses[i].region == player.region &&
        player.houses[i].rx == player.land_x &&
        player.houses[i].ry == player.land_y)
      player.map[player.houses[i].cx][player.houses[i].cy] = S_SOLD;
  clrscr();
  show_map();
  show_side();
  show_player();
  saveScreen();
  } /* void new_town(Int32 where) */

void travel(char* name) {
  char dest[9][33];
  Int32 cost[9];
  Int32 where[9];
  char flag;
  Int32  option;
  char buffer[80];
  Int32  i;
  char key;
  Int32  px,py;
  Int32  dx,dy;
  Int32  city;
  Int32  shipx,shipy;
  Int32  ndest;
  TRIPS  trips;
  MENU   menu = { .rows = { .shown = 9, .line = 2 }, .column = 27, .value_column = 50,
                  .color = 14, .keys = MENU_KEYS, .keys_line = 25, .draw_row = draw_trip, .screen = &trips };

  shipx = -1;
  shipy = -1;
  find_harbour(player.city,&shipx,&shipy);
  city = -1;
  i = 0;
  while (city == -1) {
    if (links[i].typ < 99) {
      if (links[i].region == player.region &&
          links[i].x == player.land_x &&
          links[i].y == player.land_y) city = i;
      else i++;
      } else city = -2;
    }
  if (city < 0) return;
  px = getPX(player.region,player.land_x);
  py = getPY(player.region,player.land_y);
  i = 0;
  ndest = 1;
  while (i >= 0) {
    if (i != city) {
      if (links[i].typ == 5) {
        strcpy(dest[ndest],links[i].name);
        dx = getPX(links[i].region,links[i].x);
        dy = getPY(links[i].region,links[i].y);
        cost[ndest] = abs(dx-px)*3 + abs(dy-py)*3 + links[i].premium;
        where[ndest] = i;
        ndest++;
        }
      if (links[i].typ == 99) i = -1;
        else i++;
      } else i++;
    }

  trips.dest = dest;
  trips.cost = cost;
  trips.count = ndest - 1;
  fit_menu(&menu,(shipx >= 0) ? ndest : ndest - 1);

  textmode(3);
  textmode(3); gotoxy(27,1); textcolor(11);
  sprintf(buffer,"Welcome to %s",name);
  txt_write(buffer);

  textcolor(14);
  if (improved_interface == 'Y') draw_menu(&menu);
  else {
    for (i=1; i<ndest; i++) {
      gotoxy(27,i+2);
      sprintf(buffer,"%2d> %s",i,dest[i]);
      txt_write(buffer);
      gotoxy(50,i+2);
      sprintf(buffer,"%4d",cost[i]);
      txt_write(buffer);
      }
    if (shipx >= 0)
      {
        sprintf(buffer,"%2d> %s",ndest,SHIP_LABEL);
        gotoxy(27,ndest+2); txt_write(buffer);
        gotoxy(49,11); txt_write("2500");
      }
    }

  textcolor(14); gotoxy(35,24);
  sprintf(buffer,"Gold = %d",get_gold());
  txt_write(buffer);
  flag = ' ';
  while (flag == ' ') {
    textcolor(10); gotoxy(20,ndest+4);
    if (improved_interface == 'Y') {
      txt_write("Where would you like to travel to (<ESC> to leave)?");
      option = menu_choice(&menu) + 1;
      }
    else {
      txt_write("Where would you like to travel to (<ESC> to leave) ?      ");
      gotoxy(73,ndest+4);
      strcpy(buffer,"");
      if (p_read(buffer,sizeof(buffer)) == 0) {
        sscanf(buffer,"%d",&option);
        } else option = 0;
      }
    gotoxy(20,16);
    txt_write("                                           ");
    if (option == 0) flag = '*';
    if (option == ndest && (shipx >= 0)) {
      if (player.ship_r != 0) {
        gotoxy(20,ndest+6); textcolor(14);
        txt_write("You already own a ship!!");
        delay(1000);
        }
      else {
        if (get_gold()<2500) {
          gotoxy(20,ndest+6); textcolor(14);
          txt_write("You can not afford a ship!!");
          delay(1000);
          }
        else {
          sub_gold(2500);
          player.map[shipx][shipy] = 12;

          /* Without the Ship Purchase change the ship is not owned until boarded, so another can be bought */
          if (change_ship_purchase == 'Y') {
            player.ship_x = shipx;
            player.ship_y = shipy;
            player.ship_r = ship_region();
            }
          gotoxy(20,ndest+6); textcolor(14);
          txt_write("You will find you new ship in the harbor");
          textcolor(14); gotoxy(35,24); 
          sprintf(buffer,"Gold = %d             ",get_gold());
          txt_write(buffer);
          delay(1000);
          }
        }
      }
    if (option>=1 && option<ndest) {
      gotoxy(20,ndest+6); textcolor(14);
      sprintf(buffer,"A trip to %s will cost %d",dest[option],cost[option]);
      txt_write(buffer);
      if (improved_interface == 'Y') key = ask_action(20,ndest+7,14,"Take the trip","Cancel",0);
      else {
        gotoxy(20,ndest+7); txt_write("Do you wish to go ?");
        key = inkey();
        }
      if (key == 'N' || key == 'n') {
        gotoxy(20,ndest+6); textcolor(14);
        txt_write("                                                  ");
        gotoxy(20,ndest+7); txt_write("                   ");
        }
      if (key == 'Y' || key == 'y') {
        if (cost[option]>get_gold()) {
          gotoxy(20,ndest+8); textcolor(12);
          txt_write("You do not have enough gold for that trip!!");
          gotoxy(20,ndest+6); textcolor(14);
          txt_write("                                                  ");
          gotoxy(20,ndest+7); txt_write("                   ");
          }
        else {
          sub_gold(cost[option]);
          flag = '#';
          }
        }
      }
    }
  if (flag == '#' && option>0) new_town(where[option]);
} /* void travel(char* name,Int32 city) */
