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

/* The name of a square's tile, floor item or lore square, as the look command shows it, or "" for none; an item's
   name sets color to its inventory colour */
void square_name(Int32 value,char* name,Int32* color) {
  strcpy(name,"");

  if (value >= 600 && value <= 699) strcpy(name,"Forbidding");
  else if (value >= 700 && value <= 799) strcpy(name,"Phase Door");
  else switch (value) {
    case  1:strcpy(name,"Plains"); break;
    case  2:strcpy(name,"Forest"); break;
    case  3:strcpy(name,"Mountains"); break;
    case  4:strcpy(name,"Water"); break;
    case  5:strcpy(name,"City"); break;
    case  6:strcpy(name,"Swamp"); break;
    case  8:strcpy(name,"Desert"); break;
    case  9:strcpy(name,"Hills"); break;
    case 10:strcpy(name,"Darkness"); break;
    case 11:strcpy(name,"Dungeon Entrance"); break;
    case 12:strcpy(name,"Your Ship"); break;
    case 13:strcpy(name,"Ship"); break;
    case 14:strcpy(name,"Dock"); break;
    case 16:strcpy(name,"Cactus"); break;
    case 17:strcpy(name,"Village"); break;
    case 18:strcpy(name,"Rock"); break;
    case 19:strcpy(name,"Pass"); break;
    case 20:strcpy(name,"Tree"); break;
    case 21:strcpy(name,"Wall"); break;
    case 22:strcpy(name,"Closed Door"); break;
    case 23:strcpy(name,"Open Door"); break;
    case 24:strcpy(name,"Down Stairs"); break;
    case 25:strcpy(name,"Up Stairs"); break;
    case 26:strcpy(name,"Chair"); break;
    case 27:strcpy(name,"Table"); break;
    case 28:strcpy(name,"Merchant"); break;
    case 29:strcpy(name,"Bed"); break;
    case 30:strcpy(name,"Shelves"); break;
    case 31:strcpy(name,"Gateway"); break;
    case 32:strcpy(name,"Gateway"); break;
    case 33:strcpy(name,"Jammed Door"); break;
    case 36:strcpy(name,"Sleeping"); break;
    case 38:strcpy(name,"Wall of Fire"); break;
    case 39:strcpy(name,"Wall of Water"); break;
    case 40:strcpy(name,"Disarmed Trap"); break;
    case 41:strcpy(name,"Forbidding"); break;
    case 42:strcpy(name,"Phase Door"); break;
    case 79:strcpy(name,"Gold"); break;
    case 84:strcpy(name,"Torch"); break;
    case 99:strcpy(name,"Food"); break;
    case 176:strcpy(name,"Rock"); break;
    case 178:strcpy(name,"Rock"); break;
    case 536:strcpy(name,"Gacha Machine"); break;
    case 901:strcpy(name,"Altar"); break;
    case 902:strcpy(name,"Dull Altar"); break;
    case 903:strcpy(name,"Dull Runestone"); break;
    case 904:strcpy(name,"Blue Runestone"); break;
    case 905:strcpy(name,"Red Runestone"); break;
    case 906:strcpy(name,"Purple Runestone"); break;
    case 907:strcpy(name,"White Runestone"); break;
    case 908:strcpy(name,"Yellow Runestone"); break;
    case 909:strcpy(name,"Green Runestone"); break;
    case 910:strcpy(name,"Cyan Runestone"); break;
    case 951:strcpy(name,"Armed Trap"); break;
    case 952:strcpy(name,"Armed Trap"); break;
    case 953:strcpy(name,"Armed Trap"); break;
    case 954:strcpy(name,"Armed Trap"); break;
    case 955:strcpy(name,"Armed Trap"); break;
    case 956:strcpy(name,"Armed Trap"); break;
    case 957:strcpy(name,"Armed Trap"); break;
    case 958:strcpy(name,"Armed Trap"); break;
    case 959:strcpy(name,"Armed Trap"); break;
    case 960:strcpy(name,"Armed Trap"); break;
    case 961:strcpy(name,"Armed Trap"); break;
    }

  /* 2000-2999 are lore squares, not floor items */
  if (value >= 2000 && value < 3000)
    strcpy(name,"Lore Book");
  else if (value >= 1000) {
    strcpy(name,get_name(&(player.map_objects[value - 1000])));
    *color = item_color(&(player.map_objects[value - 1000]));
    }
  }

void view(Int32 floor) {
  Int32  i = 0,x,y;
  Int32  key;
  Int32  color;
  char buffer[255];
/*
  char mflag;
  Int32  xdir,ydir;
  get_dir(&xdir,&ydir);
  x = player.x;
  y = player.y;
  mflag = ' ';
  while (mflag == ' ') {
    x += xdir;
    y += ydir;
    if (player.map[x][y] != floor) mflag = '*';
    if (player.map[x][y] > 800 && player.map[x][y] < 899) mflag = ' ';
    if (player.map[x][y] == 15) mflag = ' ';
    }
  target(100,-1,0);
  x = target_x;
  y = target_y;
*/
  hide_bottom_name();
  color = 15;
  msg("Press <ESC> to end view mode",15);
  x = player.x - player.map_x;
  y = player.y - player.map_y;
  key = 0;
  show_sprite(x,y,100);
  while (key != 13 && key != 27) {
    key = inkey();
    show_spot(x + player.map_x,y + player.map_y);
    show_player();
    switch (key) {
      case '2': y++;
                if (y>TILES_Y) y = TILES_Y;
                break;
      case '8': y--;
                if (y < 1) y = 1;
                break;
      case '6': x++;
                if (x>TILES_X) x = TILES_X;
                break;
      case '4': x--;
                if (x < 1) x = 1;
                break;
      case '7': x--; y--;
                if (x < 1) x = 1;
                if (y < 1) y = 1;
                break;
      case '9': x++; y--;
                if (x>TILES_X) x = TILES_X;
                if (y < 1) y = 1;
                break;
      case '1': x--; y++;
                if (x < 1) x = 1;
                if (y>TILES_Y) y = TILES_Y;
                break;
      case '3': x++; y++;
                if (x>TILES_X) x = TILES_X;
                if (y>TILES_Y) y = TILES_Y;
                break;

      }
    show_sprite(x,y,100);
    if (los(player.x,player.y,x+player.map_x,y+player.map_y,'Y')) {
      color = CLR_YELLOW;
      square_name(player.map[x+player.map_x][y+player.map_y],buffer,&color);
    if (player.map[x+player.map_x][y+player.map_y] < 0 ||
        (player.map[x+player.map_x][y+player.map_y] > 299 && player.map[x+player.map_x][y+player.map_y] < 400) ||
        (player.map[x+player.map_x][y+player.map_y] > 399 && player.map[x+player.map_x][y+player.map_y] < 500)) {
      if (player.map[x+player.map_x][y+player.map_y] < 0) i = player.monster[1][0-player.map[x+player.map_x][y+player.map_y]].sprite;
      if (player.map[x+player.map_x][y+player.map_y] > 299 && player.map[x+player.map_x][y+player.map_y] < 400)
        i = player.party[player.map[x+player.map_x][y+player.map_y] - 300].sprite;
      if (player.map[x+player.map_x][y+player.map_y] > 399 && player.map[x+player.map_x][y+player.map_y] < 500)
        i = player.monster[player.map[x+player.map_x][y+player.map_y] - 400][a_mon].sprite;
      switch (i) {
        case  101:strcpy(buffer,"Air Elemental"); break;
        case  102:strcpy(buffer,"Insectoid"); break;
        case  103:strcpy(buffer,"Rodent"); break;
        case  104:strcpy(buffer,"Large Animal"); break;
        case  105:strcpy(buffer,"Beholder Like"); break;
        case  106:strcpy(buffer,"Blob"); break;
        case  107:strcpy(buffer,"Blob"); break;
        case  108:strcpy(buffer,"Roper"); break;
        case  109:strcpy(buffer,"Bulette"); break;
        case  110:strcpy(buffer,"Centipede Type"); break;
        case  111:strcpy(buffer,"Fire Elemental"); break;
        case  112:strcpy(buffer,"Water Elemental"); break;
        case  113:strcpy(buffer,"Earth Elemental"); break;
        case  114:strcpy(buffer,"Efreeti"); break;
        case  115:strcpy(buffer,"Gargoyle Like"); break;
        case  116:strcpy(buffer,"Gelletanous Cube"); break;
        case  117:strcpy(buffer,"Griffon"); break;
        case  118:strcpy(buffer,"Ghost Like"); break;
        case  119:strcpy(buffer,"Harpy"); break;
        case  120:strcpy(buffer,"Dog Like"); break;
        case  121:strcpy(buffer,"Dwarf Like"); break;
        case  122:strcpy(buffer,"Hydra"); break;
        case  123:strcpy(buffer,"Orc Like"); break;
        case  124:strcpy(buffer,"Lamia"); break;
        case  125:strcpy(buffer,"Skeleton"); break;
        case  126:strcpy(buffer,"Lizard Like"); break;
        case  127:strcpy(buffer,"Manticore"); break;
        case  128:strcpy(buffer,"Medusa"); break;
        case  129:strcpy(buffer,"Mind Flayer"); break;
        case  130:strcpy(buffer,"Minotaur"); break;
        case  131:strcpy(buffer,"Horse Like"); break;
        case  132:strcpy(buffer,"Unicorn"); break;
        case  133:strcpy(buffer,"Pegasus"); break;
        case  134:strcpy(buffer,"Frog"); break;
        case  135:strcpy(buffer,"Giant"); break;
        case  136:strcpy(buffer,"Mummy Like"); break;
        case  137:strcpy(buffer,"Naga"); break;
        case  138:strcpy(buffer,"Nightmare"); break;
        case  139:strcpy(buffer,"Nymph Like"); break;
        case  140:strcpy(buffer,"Sprite Like"); break;
        case  141:strcpy(buffer,"Purple Worm"); break;
        case  142:strcpy(buffer,"Scorpian"); break;
        case  143:strcpy(buffer,"Snake Like"); break;
        case  144:strcpy(buffer,"Sphinx"); break;
        case  145:strcpy(buffer,"Spider"); break;
        case  146:strcpy(buffer,"Umber Hulk"); break;
        case  147:strcpy(buffer,"Will-o-Wisp"); break;
        case  148:strcpy(buffer,"Centaur"); break;
        case  149:strcpy(buffer,"Demogorgon"); break;
        case  150:strcpy(buffer,"Orcus"); break;
        case  151:strcpy(buffer,"Succubus"); break;
        case  152:strcpy(buffer,"Demon Like"); break;
        case  153:strcpy(buffer,"Dragon"); break;
        case  154:strcpy(buffer,"Dragon"); break;
        case  155:strcpy(buffer,"Dragon"); break;
        case  156:strcpy(buffer,"Dragon"); break;
        case  157:strcpy(buffer,"Dragon"); break;
        case  158:strcpy(buffer,"Dragon"); break;
        case  159:strcpy(buffer,"Dragon"); break;
        case  160:strcpy(buffer,"Dalek"); break;
        case  161:strcpy(buffer,"Female Humanoid"); break;
        case  162:strcpy(buffer,"Male Humanoid"); break;
        case  163:strcpy(buffer,"Fighter"); break;
        case  164:strcpy(buffer,"Cleric"); break;
        case  165:strcpy(buffer,"Wizard"); break;
        case  166:strcpy(buffer,"Dark Magi"); break;
        case  331:strcpy(buffer,"Sirine"); break;
        case  333:strcpy(buffer,"Orc Warchief"); break;
        case  334:strcpy(buffer,"Dragon Queen"); break;
        case  335:strcpy(buffer,"Count Tralin"); break;
        case  336:strcpy(buffer,"Forest Elemental"); break;
        case  337:strcpy(buffer,"Lamia Queen"); break;
        case  338:strcpy(buffer,"Kobold Lord"); break;
        case  339:strcpy(buffer,"Kobold"); break;
        case  340:strcpy(buffer,"Caldnar"); break;
        case  341:strcpy(buffer,"Elanther"); break;
        case  342:strcpy(buffer,"Summoned Snake"); break;
        }
      }
      } else strcpy(buffer,"Out of sight");
    show_bottom_name(buffer,color);
    flush();
    }
  hide_bottom_name();
  show_spot(x + player.map_x,y + player.map_y);
  show_player();
  }
