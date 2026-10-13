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

/* The crafting screen's state: the recipes the player knows and the items placed on the bench */
typedef struct {
  Int32       recipe_book[1000];
  Int32       num_recipes;
  Int32       current_recipe;
  OBJECT_REC* craft;
  OBJECT_REC* ing[10];
  } CRAFT_BENCH;

static Int32 already_taken(CRAFT_BENCH* bench,OBJECT_REC* item) {
  Int32 i;
  for (i=0; i<10; i++)
    if (bench->ing[i] != NULL && bench->ing[i] == item) return -1;
  return 0;
  }

static OBJECT_REC* find_ingredient(CRAFT_BENCH* bench,Int32 link) {
  Int32 i;
  Int32 j;
  for (i=1; i<20; i++)
    if (player.invent[i].otype != O_NONE &&
        player.invent[i].link == link &&
        already_taken(bench,&(player.invent[i])) == 0)
      return &(player.invent[i]);
  for (i=1; i<33; i++)
    if (player.pack[i].otype != O_NONE &&
        player.pack[i].link == link &&
        already_taken(bench,&(player.pack[i])) == 0)
      return &(player.pack[i]);
  for (i=0; i<1000; i++)
    if (player.hole[i].otype != O_NONE &&
        player.hole[i].link == link &&
        already_taken(bench,&(player.hole[i])) == 0)
      return &(player.hole[i]);
  for (i=0; i<5; i++)
    if (player.region == player.houses[i].region &&
        player.land_x == player.houses[i].rx &&
        player.land_y == player.houses[i].ry &&
        player.x >= player.houses[i].cx-1 &&
        player.x <= player.houses[i].cx+1 &&
        player.y >= player.houses[i].cy-1 &&
        player.y <= player.houses[i].cy+1) {
      for (j=0; j<500; j++)
        if (player.houses[i].inventory[j].otype != O_NONE &&
            player.houses[i].inventory[j].link == link &&
            already_taken(bench,&(player.houses[i].inventory[j])) == 0)
          return &(player.houses[i].inventory[j]);
      }
  return NULL;
  }

static void auto_add(CRAFT_BENCH* bench) {
  Int32 recipe;
  recipe = bench->recipe_book[bench->current_recipe];
  bench->craft = (recipes[recipe].craft_item > 0) ? find_ingredient(bench,recipes[recipe].craft_item) : NULL;
  bench->ing[0] = (recipes[recipe].object1 > 0) ? find_ingredient(bench,recipes[recipe].object1) : NULL;
  bench->ing[1] = (recipes[recipe].object2 > 0) ? find_ingredient(bench,recipes[recipe].object2) : NULL;
  bench->ing[2] = (recipes[recipe].object3 > 0) ? find_ingredient(bench,recipes[recipe].object3) : NULL;
  bench->ing[3] = (recipes[recipe].object4 > 0) ? find_ingredient(bench,recipes[recipe].object4) : NULL;
  bench->ing[4] = (recipes[recipe].object5 > 0) ? find_ingredient(bench,recipes[recipe].object5) : NULL;
  bench->ing[5] = (recipes[recipe].object6 > 0) ? find_ingredient(bench,recipes[recipe].object6) : NULL;
  bench->ing[6] = (recipes[recipe].object7 > 0) ? find_ingredient(bench,recipes[recipe].object7) : NULL;
  bench->ing[7] = (recipes[recipe].object8 > 0) ? find_ingredient(bench,recipes[recipe].object8) : NULL;
  bench->ing[8] = (recipes[recipe].object9 > 0) ? find_ingredient(bench,recipes[recipe].object9) : NULL;
  bench->ing[9] = (recipes[recipe].object10 > 0) ? find_ingredient(bench,recipes[recipe].object10) : NULL;
  }

/* The item a recipe names by link, at column and line, named as recipe_string() names it. A recipe names a kind of
   item, so the name shows in the rarity colour of one identified. Returns 0 when no item has that link. */
static Int32 draw_recipe_item(Int32 link,Int32 column,Int32 line) {
  OBJECT_REC* item;
  OBJECT_REC  known;
  char        name[80];
  Int32       i;

  item = find_linked_item(link);
  if (item == NULL) return 0;

  known = *item;
  known.flags |= OF_IDENT;
  strcpy(name,item->name);
  for (i=0; name[i] != 0; i++)
    if (name[i] == '_') name[i] = ' ';

  gotoxy(column,line);
  write_named("",&known,name,"",3);
  return 1;
  }

/* The recipe at the bench: what it makes, the item it crafts from, and its ingredients */
static void show_recipe(CRAFT_BENCH* bench) {
  Int32 i;
  Int32 line;
  Int32 recipe;
  if (bench->num_recipes == 0) return;
  recipe = bench->recipe_book[bench->current_recipe];
  for (i=1; i<20; i++) {
    gotoxy(40,i); txt_write("                                        ");
    }
  textcolor(15); gotoxy(46,2); txt_write("Recipe");
  if (!draw_recipe_item(recipes[recipe].object,46,4)) return;

  textcolor(2); gotoxy(46,6); txt_write("Craft Item:");
  if (recipes[recipe].craft_item == 0) {
    textcolor(3); gotoxy(46,7); txt_write("  none");
    }
  else if (!draw_recipe_item(recipes[recipe].craft_item,48,7)) return;

  textcolor(2); gotoxy(46,9); txt_write("Ingredients:");
  line = 10;
  for (i=0; i<10; i++)
    if (recipe_ingredient(recipe,i) != 0 && draw_recipe_item(recipe_ingredient(recipe,i),48,line)) line++;
  }

static void craft_show_screen(CRAFT_BENCH* bench) {
  Int32 i;
  textmode(3);
  textcolor(15); gotoxy(10,4); txt_write("Crafting");
  textcolor(2); gotoxy(10,6); txt_write("Craft Item:");
  if (bench->craft != NULL && bench->craft->otype != O_NONE) {
    gotoxy(10,7); write_named("",bench->craft,get_name(bench->craft),"",3);
    }
  textcolor(2); gotoxy(10,9); txt_write("Ingredients:");
  for (i=0; i<10; i++) {
    textcolor(3); gotoxy(10,i+10);
    if (bench->ing[i] != NULL && bench->ing[i]->otype != O_NONE)
      write_named("",bench->ing[i],get_name(bench->ing[i]),"",3);
    }

  textcolor(10);
  gotoxy(28,21);
  sprintf(buffer,"Recipes known : %d of %d",bench->num_recipes,recipe_count);
  txt_write(buffer);

  textcolor(6);
  gotoxy(10,23);
  txt_write("<");
  textcolor(14); txt_write("ESC");
  textcolor(6); txt_write(">quit, <");
  textcolor(14); txt_write("ENTER");
  textcolor(6); txt_write(">Select Craft Item/Ingredient, <");
  textcolor(14); txt_write("M");
  textcolor(6); txt_write(">ake Item");
  gotoxy(10,24);
  textcolor(6);
  txt_write("<");
  textcolor(14); txt_write("PGUP");
  textcolor(6); txt_write("> and <");
  textcolor(14); txt_write("PGDN");
  textcolor(6); txt_write("> will scroll through known recipes, <");
  textcolor(14); txt_write("A");
  textcolor(6); txt_write(">uto");
  show_recipe(bench);
  }

static char check_recipe(CRAFT_BENCH* bench,RECIPE rec) {
  Int32 i;
  Int32 j;
  Int32 l;
  char flag;
  char used[10];
  l = 0;
  if (rec.craft_item != 0) {
    if (bench->craft == NULL) return 'N';
    if (bench->craft->link != rec.craft_item) return 'N';
    }
  for (i=0; i<10; i++)
    if (bench->ing[i] != NULL) used[i] = 'N';
      else used[i] = '-';
  for (i=0; i<10; i++) {
    switch (i) {
      case 0:l = rec.object1; break;
      case 1:l = rec.object2; break;
      case 2:l = rec.object3; break;
      case 3:l = rec.object4; break;
      case 4:l = rec.object5; break;
      case 5:l = rec.object6; break;
      case 6:l = rec.object7; break;
      case 7:l = rec.object8; break;
      case 8:l = rec.object9; break;
      case 9:l = rec.object10; break;
      }
    flag = 'N';
    j = 0;
    while (j < 10) {
      if (used[j] == 'N' && bench->ing[j]->link == l) {
        used[j] = 'Y';
        j = 11;
        flag = 'Y';
        }
      j++;
      }
    if (l != 0 && flag == 'N') return 'N';
    }
  flag = 'Y';
  for (i=0; i<10; i++)
    if (used[i] == 'N') return 'N';
  return 'Y';
  }

static Int32 find_recipe(CRAFT_BENCH* bench) {
  Int32 i;
  i = 0;
  while (recipes[i].craft_item != 999) {
    if (check_recipe(bench,recipes[i]) == 'Y') return i;
    i++;
    }
  return -1;
  }

static void next_recipe(CRAFT_BENCH* bench) {
  if (bench->num_recipes == 0) return;
  bench->current_recipe++;
  if (bench->current_recipe >= bench->num_recipes) bench->current_recipe = 0;
  }

static void prev_recipe(CRAFT_BENCH* bench) {
  if (bench->num_recipes == 0) return;
  bench->current_recipe--;
  if (bench->current_recipe < 0) bench->current_recipe = bench->num_recipes-1;
  }

void crafting() {
  Int32 i;
  Int32 tmp;
  Int32 line;
  char flag;
  char key;
  CRAFT_BENCH bench;
  OBJECT_REC* pItem;
  OBJECT_REC  item;
  char name1[1024];
  char name2[1024];

  bench.num_recipes = 0;
  for (i=0; i<recipe_count; i++)
    if (player.lore[i+RECIPE_BASE] != 0) bench.recipe_book[bench.num_recipes++] = i;
  if (bench.num_recipes > 2) {
    flag = '*';
    while (flag == '*') {
      flag = ' ';
      for (i=0; i<bench.num_recipes-1; i++) {
        recipe_string(bench.recipe_book[i],name1);
        recipe_string(bench.recipe_book[i+1],name2);
        if (strcmp(name1,name2) > 0) {
          tmp = bench.recipe_book[i];
          bench.recipe_book[i] = bench.recipe_book[i+1];
          bench.recipe_book[i+1] = tmp;
          flag = '*';
          }
        }
      }
    }
  for (i=0; i<10; i++) bench.ing[i] = NULL;
  bench.craft = NULL;
  bench.current_recipe = 0;
  craft_show_screen(&bench);
  line = 5;
  flag = ' ';
  textcolor(14);
  gotoxy(2,line+2); txt_write("->");
  while (flag == ' ') {
    key = inkey();
    if (key == '2' || key == '.' || key == '>') {
      textcolor(14);
      gotoxy(2,line+2); txt_write("  ");
      line++;
      if (line == 6) line = 8;
      if (line > 17) line = 5;
      gotoxy(2,line+2); txt_write("->");
      }
    if (key == '8' || key == ',' || key == '<') {
      textcolor(14);
      gotoxy(2,line+2); txt_write("  ");
      line--;
      if (line == 7) line = 5;
      if (line < 5) line = 17;
      gotoxy(2,line+2); txt_write("->");
      }
    if (key == '3') {
      next_recipe(&bench);
      show_recipe(&bench);
      }
    if (key == '9') {
      prev_recipe(&bench);
      show_recipe(&bench);
      }
    if ((key == 'a' || key == 'A') && bench.num_recipes > 0) {
      auto_add(&bench);
      craft_show_screen(&bench);
      textcolor(14);
      gotoxy(2,line+2); txt_write("->");
      }
    if (key == 'M') flag = 'Y';
    if (key == 'm') flag = 'Y';
    if (key == 13) {
      pItem = unpack(-1);
      if (pItem != NULL) {
        for (i=0; i<10; i++)
          if (pItem == bench.ing[i]) bench.ing[i] = NULL;
        if (pItem == bench.craft) bench.craft = NULL;
        if (line == 5) bench.craft = pItem;
          else bench.ing[line-8] = pItem;
        }
      craft_show_screen(&bench);
      textcolor(14);
      gotoxy(2,line+2); txt_write("->");
      }
    if (key == 27) {
      flag = '*';
      }
    }
  if (flag == 'Y' && bench.num_recipes > 0) {
    line = find_recipe(&bench);
    if (line >= 0) {
      for (i=0; i<10; i++) if (bench.ing[i] != NULL) bench.ing[i]->otype = O_NONE;
      pItem = find_linked_item(recipes[line].object);
      if (pItem != NULL) {
        item = *pItem;
        item.flags |= OF_IDENT;
        inventory(&item);
	stats.counts[STAT_ITEMS_CRAFTED]++;
        }
      } else {
      textcolor(12);
      gotoxy(20,21);
      txt_write("Failed!!!");
      delay(5000);
      }
    }

  }
