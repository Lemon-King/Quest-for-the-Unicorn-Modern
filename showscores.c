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


typedef struct {
  char legend_name[NUM_CLASSES+1][33];
  char legend_title[NUM_CLASSES+1][33];
  double legend_exp[NUM_CLASSES+1];
  char high_name[NUM_CLASSES+1][33];
  char high_title[NUM_CLASSES+1][33];
  double high_exp[NUM_CLASSES+1];
  } SCORE_REC_OLD;

typedef struct {
  char    name[33];
  double  exp;
  Int32     level;
  char    cause;
  byte    class;
  Int32 time_played;
  } SCORE_ENTRY_OLD;


void save_scores() {
  Int32 i;
  FILE* file;
  file = fopen("unicorn.sco","w");
  fprintf(file,"v7.0.0\n");
  fprintf(file,"HighScores {\n");
    for (i=0;i < NUM_CLASSES; i++) {
      fprintf(file,"  Class %d {\n",i);
      fprintf(file,"    Name %s\n", high_scores[i].name);
      fprintf(file,"    Title %s\n", high_scores[i].title);
      fprintf(file,"    Exp %f\n", high_scores[i].exp);
      fprintf(file,"    Mode %c\n", high_scores[i].mode);
      fprintf(file,"    }\n");
      }
  fprintf(file,"  }\n");

  fprintf(file,"Legends {\n");
    for (i=0;i < NUM_CLASSES; i++) {
      fprintf(file,"  Class %d {\n",i);
      fprintf(file,"    Name %s\n", legend_scores[i].name);
      fprintf(file,"    Title %s\n", legend_scores[i].title);
      fprintf(file,"    Exp %f\n", legend_scores[i].exp);
      fprintf(file,"    Mode %c\n", legend_scores[i].mode);
      fprintf(file,"    }\n");
      }
  fprintf(file,"  }\n");

  fprintf(file,"HallOfFame {\n");
    for (i=0;i < num_hall_of_fame; i++) {
      fprintf(file,"  Entry %d {\n",i);
      fprintf(file,"    Name %s\n", hall_of_fame[i].name);
      fprintf(file,"    Exp %f\n", hall_of_fame[i].exp);
      fprintf(file,"    Level %d\n", hall_of_fame[i].level);
      fprintf(file,"    Cause %c\n", hall_of_fame[i].cause);
      fprintf(file,"    Class %d\n", hall_of_fame[i].class);
      fprintf(file,"    Time %d\n", hall_of_fame[i].time_played);
      fprintf(file,"    Mode %c\n", hall_of_fame[i].mode);
      fprintf(file,"    }\n");
      }
  fprintf(file,"  }\n");

  fprintf(file,"FinalScores {\n");
    for (i=0;i < num_final_scores; i++) {
      fprintf(file,"  Entry %d {\n",i);
      fprintf(file,"    Name %s\n", final_scores[i].name);
      fprintf(file,"    Exp %f\n", final_scores[i].exp);
      fprintf(file,"    Level %d\n", final_scores[i].level);
      fprintf(file,"    Cause %c\n", final_scores[i].cause);
      fprintf(file,"    Class %d\n", final_scores[i].class);
      fprintf(file,"    Time %d\n", final_scores[i].time_played);
      fprintf(file,"    Mode %c\n", final_scores[i].mode);
      fprintf(file,"    }\n");
      }
  fprintf(file,"  }\n");

  fclose(file);
  }

void clear_scores() {
  Int32 i;
  for (i=0; i<NUM_CLASSES; i++) {
    strcpy(high_scores[i].name,"");
    strcpy(high_scores[i].title,"");
    high_scores[i].exp = 0.0;
    high_scores[i].mode = 'N';
    strcpy(legend_scores[i].name,"");
    strcpy(legend_scores[i].title,"");
    legend_scores[i].exp = 0.0;
    legend_scores[i].mode = 'N';
    }
  for (i=0; i<NUM_CLASSES; i++) {
    strcpy(legend_scores[i].name,default_scores[i].name);
    strcpy(legend_scores[i].title,default_scores[i].title);
    legend_scores[i].exp = default_scores[i].exp;
    legend_scores[i].mode = 'N';
    }
  for (i=0; i<MAX_HALL_OF_FAME; i++) {
    strcpy(hall_of_fame[i].name,"");
    hall_of_fame[i].exp = 0;
    hall_of_fame[i].level = 0;
    hall_of_fame[i].cause = ' ';
    hall_of_fame[i].class = 0;
    hall_of_fame[i].time_played = 0;
    hall_of_fame[i].mode = 'N';
    }
  for (i=0; i<MAX_FINAL_SCORES; i++) {
    strcpy(final_scores[i].name,"");
    final_scores[i].exp = 0;
    final_scores[i].level = 0;
    final_scores[i].cause = ' ';
    final_scores[i].class = 0;
    final_scores[i].time_played = 0;
    final_scores[i].mode = 'N';
    }
  }

void convert_600_to_new() {
  Int32 i;
  clear_scores();
  for (i=0; i<12; i++) {
    strcpy(high_scores[i].name,scores_600.high_name[i+1]);
    strcpy(high_scores[i].title,scores_600.high_title[i+1]);
    high_scores[i].exp = scores_600.high_exp[i+1];
    high_scores[i].mode = scores_600.high_mode[i+1];
    strcpy(legend_scores[i].name,scores_600.legend_name[i+1]);
    strcpy(legend_scores[i].title,scores_600.legend_title[i+1]);
    legend_scores[i].exp = scores_600.legend_exp[i+1];
    legend_scores[i].mode = scores_600.legend_mode[i+1];
    }
  }

void load_old_scores() {
  Int32 i;
  FILE* sc_file;
  SCORE_REC_OLD scores_old;
  SCORE_ENTRY_OLD score_entry_old;
  sc_file = fopen("unicorn.sco","rb");
  if (sc_file == NULL) return;
  fread(&scores_old,sizeof(SCORE_REC_OLD),1,sc_file);
  scores_600.version[0] = '6'; scores_600.version[1] = '0'; scores_600.version[2] = '0';
  for (i=0; i<NUM_CLASSES-1; i++) {
    strcpy(scores_600.legend_name[i+1], scores_old.legend_name[i+1]);
    strcpy(scores_600.legend_title[i+1], scores_old.legend_title[i+1]);
    scores_600.legend_exp[i+1] = scores_old.legend_exp[i+1];
    scores_600.legend_mode[i+1] = 'N';
    }
  for (i=1; i<=11; i++) {
    strcpy(scores_600.high_name[i], scores_old.high_name[i]);
    strcpy(scores_600.high_title[i], scores_old.high_title[i]);
    scores_600.high_exp[i] = scores_old.high_exp[i];
    scores_600.high_mode[i] = 'N';
    }
  convert_600_to_new();
  fread(&num_hall_of_fame,sizeof(num_hall_of_fame),1,sc_file);
  fread(&num_final_scores,sizeof(num_final_scores),1,sc_file);
  for (i=0; i<num_hall_of_fame; i++) {
    fread(&score_entry_old,sizeof(SCORE_ENTRY_OLD),1,sc_file);
    strcpy(hall_of_fame[i].name,score_entry_old.name);
    hall_of_fame[i].exp = score_entry_old.exp;
    hall_of_fame[i].level = score_entry_old.level;
    hall_of_fame[i].cause = score_entry_old.cause;
    hall_of_fame[i].class = score_entry_old.class;
    hall_of_fame[i].time_played = score_entry_old.time_played;
    hall_of_fame[i].mode = 'N';
    }
  for (i=0; i<num_final_scores; i++) {
    fread(&score_entry_old,sizeof(SCORE_ENTRY_OLD),1,sc_file);
    strcpy(final_scores[i].name,score_entry_old.name);
    final_scores[i].exp = score_entry_old.exp;
    final_scores[i].level = score_entry_old.level;
    final_scores[i].cause = score_entry_old.cause;
    final_scores[i].class = score_entry_old.class;
    final_scores[i].time_played = score_entry_old.time_played;
    final_scores[i].mode = 'N';
    }
  fclose(sc_file);
  }

void load_scores_600() {
  Int32 i;
  Int32 count;
  FILE* sc_file;
  clear_scores();
  if (exist("unicorn.sco")) {
    sc_file = fopen("unicorn.sco","rb");
    if (sc_file == NULL) return;
    fread(&score_version,sizeof(SCORE_VER),1,sc_file);
    if (score_version.version[0] == '6' && score_version.version[1] == '0' && score_version.version[2] == '0') {
      fseek(sc_file,0,SEEK_SET);
      fread(&scores_600,sizeof(SCORE_REC_600),1,sc_file);
      convert_600_to_new();
      fread(&num_hall_of_fame,sizeof(num_hall_of_fame),1,sc_file);
      fread(&num_final_scores,sizeof(num_final_scores),1,sc_file);
      for (i=0; i<num_hall_of_fame; i++)
        fread(&(hall_of_fame[i]),sizeof(SCORE_ENTRY),1,sc_file);
      for (i=0; i<num_final_scores; i++)
        fread(&(final_scores[i]),sizeof(SCORE_ENTRY),1,sc_file);
      fclose(sc_file);
      }
    else if (score_version.version[0] == '6' && score_version.version[1] == '0' && score_version.version[2] == '1') {
      fread(&count,sizeof(count),1,sc_file);
      for (i=0; i<count; i++) {
        fread(&(high_scores[i]),sizeof(SCORE_RECORD),1,sc_file);
        fread(&(legend_scores[i]),sizeof(SCORE_RECORD),1,sc_file);
        }
      fread(&num_hall_of_fame,sizeof(num_hall_of_fame),1,sc_file);
      fread(&num_final_scores,sizeof(num_final_scores),1,sc_file);
      for (i=0; i<num_hall_of_fame; i++)
        fread(&(hall_of_fame[i]),sizeof(SCORE_ENTRY),1,sc_file);
      for (i=0; i<num_final_scores; i++)
        fread(&(final_scores[i]),sizeof(SCORE_ENTRY),1,sc_file);
      fclose(sc_file);
      }
    else {
      fclose(sc_file);
      load_old_scores();
      }
    }
  else { 
    score_version.version[0] = '6'; score_version.version[1] = '0'; score_version.version[2] = '1';
    for (i=0; i<NUM_CLASSES; i++) {
      strcpy(legend_scores[i].name,default_scores[i].name);
      strcpy(legend_scores[i].title,default_scores[i].title);
      legend_scores[i].exp = default_scores[i].exp;
      legend_scores[i].mode = 'N';
      }
    for (i=0; i<NUM_CLASSES; i++) {
      high_scores[i].exp = 0;
      strcpy(high_scores[i].name," ");
      strcpy(high_scores[i].title," ");
      high_scores[i].mode = 'N';
      }
    for (i=0; i<MAX_HALL_OF_FAME; i++) {
      strcpy(hall_of_fame[i].name,"");
      hall_of_fame[i].exp = 0;
      hall_of_fame[i].level = 0;
      hall_of_fame[i].cause = ' ';
      hall_of_fame[i].class = 0;
      hall_of_fame[i].mode = 'N';
      }
    for (i=0; i<MAX_FINAL_SCORES; i++) {
      strcpy(final_scores[i].name,"");
      final_scores[i].exp = 0;
      final_scores[i].level = 0;
      final_scores[i].cause = ' ';
      final_scores[i].class = 0;
      final_scores[i].mode = 'N';
      }
    save_scores();
    }
  }

void read_save_file(FILE* file, char* buffer) {
  Int32 i;
  Int32 j;
  fgets(buffer, 1024, file);
  for (i=0; i<strlen(buffer); i++)
    if (buffer[i] > 0 && buffer[i] < ' ') buffer[i] = 0;
  while (buffer[0] == ' ') for (j=0; j<strlen(buffer); j++) buffer[j] = buffer[j+1];
  }

void load_scores() {
  Int32 ver;
  Int32 change_set;
  Int32 class;
  byte  flag;
  byte  flag2;
  char  line[1024];
  char* next;
  FILE* file;
  if (exist("unicorn.sco")) {
    file = fopen("unicorn.sco","r");
    if (file == NULL) return;
    ver = fgetc(file);
    if (ver != 'v' && ver != 'V') {
      fclose(file);
      load_scores_600();
      return;
      }
    rewind(file);
    change_set = -1;
    read_save_file(file, line);
    if (strcmp(line, "v7.0.0") == 0) change_set = 0;
    if (change_set < 0) {
      printf("Error:  Save file is unknown format.  Ignoring it.\n");
      fclose(file);
      return;
      }
    while (!feof(file)) {
      read_save_file(file, line);
      if (match(line, "HighScores {")) {
        flag = 0xff;
        while (flag) {
          read_save_file(file, line);
          if (match(line, "Class ")) {
            next = next_word(line);
            class = atoi(next);
            flag2 = 0xff;
            while (flag2) {
              read_save_file(file, line);
              if (match(line, "Name ")) strcpy(high_scores[class].name, next_word(line));
              if (match(line, "Title ")) strcpy(high_scores[class].title, next_word(line));
              if (match(line, "Exp ")) high_scores[class].exp = atof(next_word(line));
              if (match(line, "Mode ")) high_scores[class].mode = next_word(line)[0];
              if (match(line, "}")) flag2 = 0;
              if (feof(file)) flag2 = 0;
              }
            }
          else if (match(line, "}")) flag = 0;
          if (feof(file)) flag = 0;
          }
        }
      if (match(line, "Legends {")) {
        flag = 0xff;
        while (flag) {
          read_save_file(file, line);
          if (match(line, "Class ")) {
            next = next_word(line);
            class = atoi(next);
            flag2 = 0xff;
            while (flag2) {
              read_save_file(file, line);
              if (match(line, "Name ")) strcpy(legend_scores[class].name, next_word(line));
              if (match(line, "Title ")) strcpy(legend_scores[class].title, next_word(line));
              if (match(line, "Exp ")) legend_scores[class].exp = atof(next_word(line));
              if (match(line, "Mode ")) legend_scores[class].mode = next_word(line)[0];
              if (match(line, "}")) flag2 = 0;
              if (feof(file)) flag2 = 0;
              }
            }
          else if (match(line, "}")) flag = 0;
          if (feof(file)) flag = 0;
          }
        }
      if (match(line, "FinalScores {")) {
        flag = 0xff;
        while (flag) {
          read_save_file(file, line);
          if (match(line, "Entry ")) {
            next = next_word(line);
            class = atoi(next);
            if (class >= num_final_scores) num_final_scores = class + 1;
            flag2 = 0xff;
            while (flag2) {
              read_save_file(file, line);
              if (match(line, "Name ")) strcpy(final_scores[class].name, next_word(line));
              if (match(line, "Exp ")) final_scores[class].exp = atof(next_word(line));
              if (match(line, "Level ")) final_scores[class].level = atoi(next_word(line));
              if (match(line, "Class ")) final_scores[class].class = atoi(next_word(line));
              if (match(line, "Time ")) final_scores[class].time_played = atoi(next_word(line));
              if (match(line, "Cause ")) final_scores[class].cause = next_word(line)[0];
              if (match(line, "Mode ")) final_scores[class].mode = next_word(line)[0];
              if (match(line, "}")) flag2 = 0;
              if (feof(file)) flag2 = 0;
              }
            }
          else if (match(line, "}")) flag = 0;
          if (feof(file)) flag = 0;
          }
        }
      if (match(line, "HallOfFame {")) {
        flag = 0xff;
        while (flag) {
          read_save_file(file, line);
          if (match(line, "Entry ")) {
            next = next_word(line);
            class = atoi(next);
            if (class >= num_hall_of_fame) num_hall_of_fame = class + 1;
            flag2 = 0xff;
            while (flag2) {
              read_save_file(file, line);
              if (match(line, "Name ")) strcpy(hall_of_fame[class].name, next_word(line));
              if (match(line, "Exp ")) hall_of_fame[class].exp = atof(next_word(line));
              if (match(line, "Level ")) hall_of_fame[class].level = atoi(next_word(line));
              if (match(line, "Class ")) hall_of_fame[class].class = atoi(next_word(line));
              if (match(line, "Time ")) hall_of_fame[class].time_played = atoi(next_word(line));
              if (match(line, "Cause ")) hall_of_fame[class].cause = next_word(line)[0];
              if (match(line, "Mode ")) hall_of_fame[class].mode = next_word(line)[0];
              if (match(line, "}")) flag2 = 0;
              if (feof(file)) flag2 = 0;
              }
            }
          else if (match(line, "}")) flag = 0;
          if (feof(file)) flag = 0;
          }
        }
// else
      }
    fclose(file);
    }
    else {
    printf("Score file not found. Building a new one\n");
    clear_scores();
    }
  }

void show_final_scores() {
  Int32 i;
  Int32 first;
  char key;
  char buffer[34];
  textmode(3);
  textcolor(10); gotoxy(32,1); txt_write("Hall of Legends:"); textcolor(2);
  gotoxy(1,2); txt_write(" Place   Name            Time                Class       Level           Exp");
  if (hall_position < 0) first = 0;
  if (hall_position > 9) first = hall_position - 4;
  if (hall_position >=0 && hall_position < 10) first = 0;
  if (first > MAX_HALL_OF_FAME-10) first = MAX_HALL_OF_FAME-10;
  for (i=0; i<10; i++) {
    if (hall_of_fame[i+first].exp > 0) {
      if (i+first == hall_position) textcolor(11); else textcolor(14);
      sprintf(buffer,"%5d",i+first+1);
      gotoxy(1,i+3); txt_write(buffer);
      gotoxy(10,i+3); txt_write(hall_of_fame[i+first].name);
      convert_time(hall_of_fame[i+first].time_played,buffer);
      gotoxy(28,i+3); txt_write(buffer);
      if (hall_of_fame[i+first].class > NUM_CLASSES) strcpy(buffer,"NONE");
        else strcpy(buffer,classes[hall_of_fame[i+first].class-1].name);
      gotoxy(46,i+3); txt_write(buffer);
      sprintf(buffer,"%d",hall_of_fame[i+first].level);
      gotoxy(61,i+3); txt_write(buffer);
      if (hall_of_fame[i+first].mode == 'H') textcolor(12);
      if (hall_of_fame[i+first].mode == 'E') textcolor(13);
      sprintf(buffer,"%12.0f",hall_of_fame[i+first].exp);
      gotoxy(65,i+3); txt_write(buffer);
      }
    }
  textcolor(10); gotoxy(30,13); txt_write("Hall of Remembrance:"); textcolor(2);
  gotoxy(1,14); txt_write(" Place   Name              Time              Class       Level           Exp");
  if (death_position < 0) first = 0;
  if (death_position > 9) first = death_position - 4;
  if (death_position >=0 && death_position < 10) first = 0;
  if (first > MAX_FINAL_SCORES-10) first = MAX_FINAL_SCORES-10;
  for (i=0; i<10; i++) {
    if (final_scores[i+first].exp > 0) {
      if (i+first == death_position) textcolor(11); else textcolor(14);
      if (i+first == score_position) {
        gotoxy(1,i+15); txt_write("*");
        }
      sprintf(buffer,"%5d",i+first+1);
      gotoxy(1,i+15); txt_write(buffer);
      if (i+first == score_position) {
        gotoxy(1,i+15); txt_write("*");
        }
      gotoxy(10,i+15); txt_write(final_scores[i+first].name);
      convert_time(final_scores[i+first].time_played,buffer);
      gotoxy(28,i+15); txt_write(buffer);
      if (final_scores[i+first].class > NUM_CLASSES) strcpy(buffer,"NONE");
        else strcpy(buffer,classes[final_scores[i+first].class-1].name);
      gotoxy(46,i+15); txt_write(buffer);
      sprintf(buffer,"%d",final_scores[i+first].level);
      gotoxy(61,i+15); txt_write(buffer);
      if (final_scores[i+first].mode == 'H') textcolor(12);
      if (final_scores[i+first].mode == 'E') textcolor(13);
      sprintf(buffer,"%12.0f",final_scores[i+first].exp);
      gotoxy(65,i+15); txt_write(buffer);
      }
    }
  key = 0;
  while (key != 13) key = inkey();
  }

void show_scores() {
  Int32 i;
  char key;
  char buffer[80];
  textmode(3);
  textcolor(10); gotoxy(1,1); txt_write("High Scores:"); textcolor(2);
  for (i=0; i<NUM_CLASSES; i++) {
    gotoxy(5,2+i); txt_write(classes[i].name);
    gotoxy(16,2+i); txt_write(":");
    }
  for (i=0; i<NUM_CLASSES; i++) {
    if (score2-1 == i) textcolor(11); else textcolor(14);
    gotoxy(18,i+2);
    sprintf(buffer,"%10.0f",high_scores[i].exp);
    txt_write(buffer);
    gotoxy(30,i+2); txt_write(high_scores[i].name);
    gotoxy(55,i+2); txt_write(high_scores[i].title);
    }
  key = 0;
  while (key != 13) key = inkey();
  }


void show_legends() {
  Int32 i;
  char key;
  char buffer[80];
  textmode(3);
  textcolor(10); gotoxy(1,1); txt_write("Legends:"); textcolor(2);
  for (i=0; i<NUM_CLASSES; i++) {
    gotoxy(5,2+i); txt_write(classes[i].name);
    gotoxy(16,2+i); txt_write(":");
    }
  for (i=0; i<NUM_CLASSES; i++) {
    if (score1-1 == i) textcolor(11); else textcolor(14);
    gotoxy(18,i+2);
    sprintf(buffer,"%10.0f",legend_scores[i].exp);
    txt_write(buffer);
    gotoxy(30,i+2); txt_write(legend_scores[i].name);
    gotoxy(55,i+2); txt_write(legend_scores[i].title);
    }
  key = 0;
  while (key != 13) key = inkey();
  }

