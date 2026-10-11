/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* Writes one setting into a .cfg file, keeping every other line, comments included, as it was. */

#include "header.h"

/* The whole file as one string, or NULL when it is missing, empty or unreadable */
static char* read_text(char* filename,size_t* length) {
  FILE* file;
  char* text;
  long  size;

  *length = 0;
  file = fopen(filename,"r");
  if (file == NULL) return NULL;

  fseek(file,0,SEEK_END);
  size = ftell(file);
  fseek(file,0,SEEK_SET);

  text = (size > 0) ? malloc((size_t)size + 1) : NULL;
  if (text != NULL) {
    *length = fread(text,1,(size_t)size,file);
    text[*length] = 0;
    }
  fclose(file);
  return text;
  }

/* 1 when process_config() would read the line as key=value */
static Int32 sets_key(char* line,char* key) {
  size_t length;

  while (*line == ' ') line++;
  length = strlen(key);
  return (_strnicmp(line,key,length) == 0 && line[length] == '=');
  }

/* Replaces each line of <name>.cfg that sets key, or adds one at the end when no line does */
void save_config_value(char* name,char* key,Int32 value) {
  FILE*  file;
  char   filename[1024];
  char*  text;
  char*  line;
  char*  end;
  size_t length;
  char   found;

  snprintf(filename,sizeof(filename),"%s.cfg",name);
  text = read_text(filename,&length);

  file = fopen(filename,"w");
  if (file == NULL) {
    free(text);
    return;
    }

  found = 'N';
  line = text;
  while (length > 0 && line < text + length) {
    end = strchr(line,'\n');
    if (end == NULL) end = text + length;

    if (sets_key(line,key)) {
      fprintf(file,"%s=%d\n",key,value);
      found = 'Y';
      }
    else {
      fwrite(line,1,(size_t)(end - line),file);
      fputc('\n',file);
      }

    line = end + 1;
    }

  if (found == 'N') fprintf(file,"%s=%d\n",key,value);
  fclose(file);
  free(text);
  }
