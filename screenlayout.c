/*
 *******************************************************************
 *** This software is copyright 1985-2014 by Michael H Riley     ***
 *** You have permission to use, modify, copy, and distribute    ***
 *** this software so long as this copyright notice is retained. ***
 *** This software may not be used in commercial applications    ***
 *** without express written permission from the author.         ***
 *******************************************************************
*/

/* The play screen's layout. The frames end above a strip along the screen's bottom edge that holds the status icons,
   the compass and the bottom name. In Icons mode the status icons show under the stats instead, so the frames reach
   the edge: the map gains the strip's tile row, and the minimap moves down, leaving the log more lines and a detail
   line for the compass and the name. */

#include "header.h"

#define FRAME_WIDTH       10
#define FRAME_ROWS        21     /* the top and bottom bands, and the row between the map and the bottom band */
#define MINIMAP_BOX      223     /* the top of the minimap's box, above the frames' bottom edge */
#define MINIMAP_TOP      212     /* the top of the minimap itself */
#define DETAIL_GAP         2     /* between the detail line and the minimap's box */
#define FIRST_BASELINE   110     /* the log's first line */
#define LOG_BOX_END      100     /* clear_messages() clears the log's box down to here, plus a line height per line */

/* A sprite drawn with put_half_image() */
Int32 half_tile_width(void) {
  return (tile_width + 1) / 2;
  }

Int32 half_tile_height(void) {
  return (tile_height + 1) / 2;
  }

Int32 has_bottom_strip(void) {
  return frame_bottom < screen_height;
  }

/* The detail line is as tall as a half-size icon */
Int32 detail_line_top(void) {
  return frame_bottom - MINIMAP_BOX - DETAIL_GAP - half_tile_height();
  }

Int32 detail_line_bottom(void) {
  return detail_line_top() + half_tile_height();
  }

/* Lays the screen out for the Improved Status mode. Returns 1 when the layout changed. */
Int32 set_screen_layout(void) {
  Int32 bottom;
  Int32 lines;

  bottom = (improved_status == 'I') ? screen_height : screen_height - tile_height;
  if (bottom == frame_bottom) return 0;

  frame_bottom = bottom;
  TILES_Y = (frame_bottom - FRAME_ROWS) / tile_height;
  smap_offset_y = frame_bottom - MINIMAP_TOP;

  if (has_bottom_strip()) lines = (smap_offset_y - FIRST_BASELINE) / MESSAGE_HEIGHT;
    else lines = (detail_line_top() - LOG_BOX_END) / MESSAGE_HEIGHT;
  if (lines > MAX_MSG) lines = MAX_MSG;
  set_log_lines(lines);

  return 1;
  }

/* The red frames around the map and the minimap */
void show_map_frames(void) {
  setfillstyle(1,12);
  bar(0,0,screen_width-220,FRAME_WIDTH);
  bar(0,frame_bottom-FRAME_WIDTH,screen_width-220,frame_bottom);
  bar(0,0,FRAME_WIDTH,frame_bottom);
  bar(screen_width-230,0,screen_width-220,frame_bottom);

  bar(screen_width-221,frame_bottom-MINIMAP_BOX,screen_width-10,frame_bottom-MINIMAP_BOX+FRAME_WIDTH);
  bar(screen_width-221,frame_bottom-FRAME_WIDTH,screen_width-10,frame_bottom);
  bar(screen_width-19,frame_bottom-MINIMAP_BOX,screen_width-10,frame_bottom);
  }
