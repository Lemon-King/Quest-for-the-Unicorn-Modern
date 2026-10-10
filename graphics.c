#ifndef ASCII

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graphics.h"

#ifdef MINGW
#include <stdint.h>
typedef int32_t  Int32;
typedef uint32_t UInt32;
#else
typedef int32_t   Int32;
typedef u_int32_t UInt32;
#endif

Int32 red_mask;
Int32 green_mask;
Int32 blue_mask;
Int32 red_shift;
Int32 green_shift;
Int32 blue_shift;

void rcs_buildColorMapping(rcs_Display* display) {
#ifndef WIN32
  Visual* visual;
  visual = DefaultVisual(display, DefaultScreen(display));
  red_mask = visual->red_mask;
  green_mask = visual->green_mask;
  blue_mask = visual->blue_mask;
  red_shift = 0;
  while (red_mask >= 0x10000) {
    red_mask >>= 1;
    red_shift--;
    }
  while (red_mask < 0x8000) {
    red_mask <<= 1;
    red_shift++;
    }
  green_shift = 0;
  while (green_mask >= 0x10000) {
    green_mask >>= 1;
    green_shift--;
    }
  while (green_mask < 0x8000) {
    green_mask <<= 1;
    green_shift++;
    }
  blue_shift = 0;
  while (blue_mask >= 0x10000) {
    blue_mask >>= 1;
    blue_shift--;
    }
  while (blue_mask < 0x8000) {
    blue_mask <<= 1;
    blue_shift++;
    }
#endif
  }

/* ******************** */
/* ***** Viewport ***** */
/* ******************** */
/* An area of pixels; right and bottom are exclusive */
typedef struct {
  Int32 left;
  Int32 top;
  Int32 right;
  Int32 bottom;
  } _rcs_Rect;

/* One axis of the buffer: its size in buffer pixels and in window pixels */
typedef struct {
  Int32 size;
  Int32 scaled;
  } _rcs_Axis;

/* Where a window pixel takes its colour along one axis: the buffer pixel it starts in and,
   out of 256, how much of it lies over the next one */
typedef struct {
  Int32 source;
  Int32 blend;
  } _rcs_Sample;

/* The 0x00RRGGBB pixels of an area, row by row */
typedef struct {
  _rcs_Rect area;
  UInt32*   pixels;
  } _rcs_Image;

struct _rcs_Viewport {
  rcs_Display* display;
  rcs_Window   window;
  rcs_Pixmap   buffer;
  _rcs_Axis    columns;
  _rcs_Axis    rows;
  Int32        windowWidth;
  Int32        windowHeight;
  Int32        left;
  Int32        top;
  _rcs_Rect    dirty;
#ifdef X
  GC           gc;
#endif
  };

/* What the window is painted through: the window itself on X11, a device context on Win32 */
#ifdef X
typedef Drawable _rcs_Surface;
static XContext  _rcs_viewports;
#endif
#ifdef WIN32
typedef HDC _rcs_Surface;
#endif

static Int32 _rcs_rectWidth(_rcs_Rect area) {
  return area.right - area.left;
  }

static Int32 _rcs_rectHeight(_rcs_Rect area) {
  return area.bottom - area.top;
  }

static void _rcs_clearDirty(rcs_Viewport v) {
  v->dirty.left = 0;
  v->dirty.top = 0;
  v->dirty.right = 0;
  v->dirty.bottom = 0;
  }

/* Window pixel i covers buffer span i*size/scaled .. (i+1)*size/scaled: at most two buffer pixels */
static _rcs_Sample _rcs_sampleAxis(_rcs_Axis axis,Int32 i) {
  _rcs_Sample sample;
  Int32       overlap;
  sample.source = i * axis.size / axis.scaled;
  overlap = (i + 1) * axis.size - (sample.source + 1) * axis.scaled;
  sample.blend = (overlap > 0) ? overlap * 256 / axis.size : 0;
  return sample;
  }

/* The window pixels that show any part of buffer pixels from .. to-1 */
static void _rcs_axisShown(_rcs_Axis axis,Int32 from,Int32 to,Int32* first,Int32* last) {
  *first = from * axis.scaled / axis.size;
  *last = (to * axis.scaled + axis.size - 1) / axis.size;
  }

/* The buffer pixels that window pixels from .. to-1 take their colour from */
static void _rcs_axisSampled(_rcs_Axis axis,Int32 from,Int32 to,Int32* first,Int32* last) {
  _rcs_Sample end;
  end = _rcs_sampleAxis(axis,to - 1);
  *first = _rcs_sampleAxis(axis,from).source;
  *last = end.source + ((end.blend > 0) ? 2 : 1);
  }

/* Fits the buffer to the window at its own aspect ratio, never below 1:1, centred. */
static void _rcs_layoutViewport(rcs_Viewport v,Int32 windowWidth,Int32 windowHeight) {
  Int32 width;
  Int32 height;
  width = v->columns.size;
  height = v->rows.size;

  if (windowWidth < width || windowHeight < height) {
    v->columns.scaled = width;
    v->rows.scaled = height;
    }
  else if (windowWidth * height <= windowHeight * width) {
    v->columns.scaled = windowWidth;
    v->rows.scaled = windowWidth * height / width;
    }
  else {
    v->columns.scaled = windowHeight * width / height;
    v->rows.scaled = windowHeight;
    }

  v->windowWidth = windowWidth;
  v->windowHeight = windowHeight;
  v->left = (windowWidth - v->columns.scaled) / 2;
  v->top = (windowHeight - v->rows.scaled) / 2;
  }

static void _rcs_fillBlack(rcs_Viewport v,_rcs_Surface surface,Int32 x,Int32 y,Int32 w,Int32 h) {
#ifdef WIN32
  RECT rect;
#endif
  if (w <= 0 || h <= 0) return;

#ifdef X
  XFillRectangle(v->display,surface,v->gc,x,y,w,h);
#endif
#ifdef WIN32
  rect.left = x;
  rect.top = y;
  rect.right = x + w;
  rect.bottom = y + h;
  FillRect(surface,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
#endif
  }

static UInt32* _rcs_allocPixels(_rcs_Rect area) {
  return (UInt32*)malloc(_rcs_rectWidth(area) * _rcs_rectHeight(area) * sizeof(UInt32));
  }

/* Mixes weight/256 of second into first, one channel at a time */
static UInt32 _rcs_blendPixel(UInt32 first,UInt32 second,Int32 weight) {
  UInt32 redBlue;
  UInt32 green;
  redBlue = ((first & 0xff00ff) * (256 - weight) + (second & 0xff00ff) * weight) >> 8;
  green = ((first & 0x00ff00) * (256 - weight) + (second & 0x00ff00) * weight) >> 8;
  return (redBlue & 0xff00ff) | (green & 0x00ff00);
  }

static UInt32 _rcs_samplePixel(UInt32* row,_rcs_Sample column) {
  if (column.blend == 0) return row[column.source];
  return _rcs_blendPixel(row[column.source],row[column.source + 1],column.blend);
  }

/* Each window pixel averages the buffer pixels it covers, so whole-number scales stay sharp and
   other scales soften only the edges between pixels. */
static Int32 _rcs_scaleImage(rcs_Viewport v,_rcs_Image* source,_rcs_Image* scaled) {
  _rcs_Sample* columns;
  _rcs_Sample  row;
  UInt32*      upper;
  UInt32*      lower;
  UInt32*      out;
  Int32        sourceWidth;
  Int32        width;
  Int32        x,y;
  sourceWidth = _rcs_rectWidth(source->area);
  width = _rcs_rectWidth(scaled->area);
  columns = (_rcs_Sample*)malloc(width * sizeof(_rcs_Sample));
  if (columns == NULL) return 0;

  for (x=0; x<width; x++) {
    columns[x] = _rcs_sampleAxis(v->columns,scaled->area.left + x);
    columns[x].source -= source->area.left;
    }

  out = scaled->pixels;
  for (y=scaled->area.top; y<scaled->area.bottom; y++) {
    row = _rcs_sampleAxis(v->rows,y);
    upper = source->pixels + (row.source - source->area.top) * sourceWidth;
    lower = upper + sourceWidth;

    for (x=0; x<width; x++) {
      if (row.blend == 0)
        *out++ = _rcs_samplePixel(upper,columns[x]);
      else
        *out++ = _rcs_blendPixel(_rcs_samplePixel(upper,columns[x]),_rcs_samplePixel(lower,columns[x]),
                                 row.blend);
      }
    }

  free(columns);
  return 1;
  }

#ifdef X
/* Inverse of rcs_RGBToPixel() for one channel, scaled to 8 bits. */
static unsigned char _rcs_pixelChannel(unsigned long pixel,Int32 mask,Int32 shift) {
  unsigned long value;
  if (shift < 0)
    value = (pixel >> -shift) & mask;
  else
    value = (pixel << shift) & mask;
  return value >> 8;
  }

static UInt32 _rcs_RGBFromPixel(unsigned long pixel) {
  return ((UInt32)_rcs_pixelChannel(pixel,red_mask,red_shift) << 16) |
         ((UInt32)_rcs_pixelChannel(pixel,green_mask,green_shift) << 8) |
         _rcs_pixelChannel(pixel,blue_mask,blue_shift);
  }

/* rcs_RGBToPixel() takes 16-bit channels */
static unsigned long _rcs_pixelFromRGB(UInt32 rgb) {
  return rcs_RGBToPixel(((rgb >> 16) & 0xff) * 257,((rgb >> 8) & 0xff) * 257,(rgb & 0xff) * 257);
  }
#endif

#ifdef WIN32
/* A top-down 32-bit DIB, whose pixels read as 0x00RRGGBB */
static void _rcs_imageInfo(BITMAPINFO* info,_rcs_Rect area) {
  memset(info,0,sizeof(BITMAPINFO));
  info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info->bmiHeader.biWidth = _rcs_rectWidth(area);
  info->bmiHeader.biHeight = -_rcs_rectHeight(area);
  info->bmiHeader.biPlanes = 1;
  info->bmiHeader.biBitCount = 32;
  info->bmiHeader.biCompression = BI_RGB;
  }
#endif

/* Reads the image's area of the buffer into its pixels. */
static Int32 _rcs_readBuffer(rcs_Viewport v,_rcs_Image* image) {
  Int32      width;
  Int32      height;
#ifdef X
  XImage*    source;
  UInt32*    out;
  Int32      x,y;
#endif
#ifdef WIN32
  BITMAPINFO info;
  HBITMAP    section;
  void*      bits;
  HDC        source;
  HDC        target;
  HGDIOBJ    previousSource;
  HGDIOBJ    previousTarget;
#endif
  width = _rcs_rectWidth(image->area);
  height = _rcs_rectHeight(image->area);

#ifdef X
  source = XGetImage(v->display,v->buffer,image->area.left,image->area.top,width,height,AllPlanes,ZPixmap);
  if (source == NULL) return 0;

  out = image->pixels;
  for (y=0; y<height; y++)
    for (x=0; x<width; x++) *out++ = _rcs_RGBFromPixel(XGetPixel(source,x,y));

  XDestroyImage(source);
#endif
#ifdef WIN32
  /* GetDIBits only reads whole rows, so the area is first copied into a DIB section */
  _rcs_imageInfo(&info,image->area);
  section = CreateDIBSection(NULL,&info,DIB_RGB_COLORS,&bits,NULL,0);
  if (section == NULL) return 0;

  source = CreateCompatibleDC(NULL);
  target = CreateCompatibleDC(NULL);
  previousSource = SelectObject(source,v->buffer);
  previousTarget = SelectObject(target,section);
  BitBlt(target,0,0,width,height,source,image->area.left,image->area.top,SRCCOPY);
  GdiFlush();
  memcpy(image->pixels,bits,width * height * sizeof(UInt32));

  SelectObject(target,previousTarget);
  SelectObject(source,previousSource);
  DeleteDC(target);
  DeleteDC(source);
  DeleteObject(section);
#endif
  return 1;
  }

/* Draws the image at its place in the window. */
static void _rcs_writeWindow(rcs_Viewport v,_rcs_Surface surface,_rcs_Image* image) {
  Int32      width;
  Int32      height;
#ifdef X
  XImage*    output;
  UInt32*    in;
  Int32      x,y;
#endif
#ifdef WIN32
  BITMAPINFO info;
#endif
  width = _rcs_rectWidth(image->area);
  height = _rcs_rectHeight(image->area);

#ifdef X
  output = XCreateImage(v->display,DefaultVisual(v->display,DefaultScreen(v->display)),
                        DefaultDepth(v->display,DefaultScreen(v->display)),ZPixmap,0,NULL,width,height,32,0);
  if (output == NULL) return;

  output->data = (char*)malloc(output->bytes_per_line * height);
  if (output->data == NULL) {
    XDestroyImage(output);
    return;
    }

  in = image->pixels;
  for (y=0; y<height; y++)
    for (x=0; x<width; x++) XPutPixel(output,x,y,_rcs_pixelFromRGB(*in++));

  XPutImage(v->display,surface,v->gc,output,0,0,v->left + image->area.left,v->top + image->area.top,
            width,height);
  XDestroyImage(output);
#endif
#ifdef WIN32
  _rcs_imageInfo(&info,image->area);
  SetDIBitsToDevice(surface,v->left + image->area.left,v->top + image->area.top,width,height,
                    0,0,0,height,image->pixels,&info,DIB_RGB_COLORS);
#endif
  }

/* At 1:1 the buffer is copied as it is. */
static void _rcs_copyUnscaled(rcs_Viewport v,_rcs_Surface surface,_rcs_Rect area) {
#ifdef WIN32
  HDC     source;
  HGDIOBJ previous;
#endif
#ifdef X
  XCopyArea(v->display,v->buffer,surface,v->gc,area.left,area.top,_rcs_rectWidth(area),_rcs_rectHeight(area),
            v->left + area.left,v->top + area.top);
#endif
#ifdef WIN32
  source = CreateCompatibleDC(surface);
  previous = SelectObject(source,v->buffer);
  BitBlt(surface,v->left + area.left,v->top + area.top,_rcs_rectWidth(area),_rcs_rectHeight(area),
         source,area.left,area.top,SRCCOPY);
  SelectObject(source,previous);
  DeleteDC(source);
#endif
  }

/* Redraws the window pixels that show a buffer area, from the buffer pixels under them. */
static void _rcs_copyScaled(rcs_Viewport v,_rcs_Surface surface,_rcs_Rect area) {
  _rcs_Image source;
  _rcs_Image scaled;
  _rcs_axisShown(v->columns,area.left,area.right,&scaled.area.left,&scaled.area.right);
  _rcs_axisShown(v->rows,area.top,area.bottom,&scaled.area.top,&scaled.area.bottom);
  _rcs_axisSampled(v->columns,scaled.area.left,scaled.area.right,&source.area.left,&source.area.right);
  _rcs_axisSampled(v->rows,scaled.area.top,scaled.area.bottom,&source.area.top,&source.area.bottom);

  source.pixels = _rcs_allocPixels(source.area);
  scaled.pixels = _rcs_allocPixels(scaled.area);
  if (source.pixels != NULL && scaled.pixels != NULL &&
      _rcs_readBuffer(v,&source) && _rcs_scaleImage(v,&source,&scaled))
    _rcs_writeWindow(v,surface,&scaled);

  free(source.pixels);
  free(scaled.pixels);
  }

/* Copies a buffer area to its place in the window. */
static void _rcs_copyToWindow(rcs_Viewport v,_rcs_Surface surface,_rcs_Rect area) {
  if (v->columns.scaled == v->columns.size && v->rows.scaled == v->rows.size)
    _rcs_copyUnscaled(v,surface,area);
  else
    _rcs_copyScaled(v,surface,area);
  }

/* Repaints the whole window: the scaled buffer framed by black borders. */
static void _rcs_paintViewport(rcs_Viewport v,_rcs_Surface surface) {
  _rcs_Rect whole;
  Int32     right;
  Int32     bottom;
  Int32     height;
  right = v->left + v->columns.scaled;
  bottom = v->top + v->rows.scaled;
  height = v->rows.scaled;

  _rcs_fillBlack(v,surface,0,0,v->windowWidth,v->top);
  _rcs_fillBlack(v,surface,0,bottom,v->windowWidth,v->windowHeight - bottom);
  _rcs_fillBlack(v,surface,0,v->top,v->left,height);
  _rcs_fillBlack(v,surface,right,v->top,v->windowWidth - right,height);

  whole.left = 0;
  whole.top = 0;
  whole.right = v->columns.size;
  whole.bottom = v->rows.size;
  _rcs_copyToWindow(v,surface,whole);
  _rcs_clearDirty(v);
  }

#ifdef X
/* Refits and repaints a viewport's window when X resizes or exposes it. */
static void _rcs_viewportEvent(rcs_Display* d,XEvent* event) {
  rcs_Viewport v;
  XPointer     data;
  if (XFindContext(d,event->xany.window,_rcs_viewports,&data) != 0) return;
  v = (rcs_Viewport)data;

  if (event->type == ConfigureNotify &&
      (event->xconfigure.width != v->windowWidth || event->xconfigure.height != v->windowHeight)) {
    _rcs_layoutViewport(v,event->xconfigure.width,event->xconfigure.height);
    _rcs_paintViewport(v,v->window);
    }

  if (event->type == Expose && event->xexpose.count == 0) _rcs_paintViewport(v,v->window);
  }
#endif

#ifdef WIN32
/* Outer window size for a client area of width x height, with the frame at the window's DPI. */
static void _rcs_frameSize(HWND win,Int32 width,Int32 height,Int32* outerWidth,Int32* outerHeight) {
  RECT frame;
  frame.left = 0;
  frame.top = 0;
  frame.right = width;
  frame.bottom = height;
  AdjustWindowRectExForDpi(&frame,GetWindowLong(win,GWL_STYLE),FALSE,GetWindowLong(win,GWL_EXSTYLE),
                           GetDpiForWindow(win));
  *outerWidth = frame.right - frame.left;
  *outerHeight = frame.bottom - frame.top;
  }

static void _rcs_setClientSize(HWND win,Int32 width,Int32 height) {
  Int32 outerWidth,outerHeight;
  _rcs_frameSize(win,width,height,&outerWidth,&outerHeight);
  SetWindowPos(win,NULL,0,0,outerWidth,outerHeight,SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
  }

/* The whole-number scale nearest the monitor's DPI at which the window still fits its work area. */
static Int32 _rcs_dpiScale(rcs_Viewport v) {
  MONITORINFO monitor;
  Int32       scale;
  Int32       outerWidth,outerHeight;
  monitor.cbSize = sizeof(monitor);
  GetMonitorInfo(MonitorFromWindow(v->window,MONITOR_DEFAULTTOPRIMARY),&monitor);
  scale = (GetDpiForWindow(v->window) + USER_DEFAULT_SCREEN_DPI / 2) / USER_DEFAULT_SCREEN_DPI;

  while (scale > 1) {
    _rcs_frameSize(v->window,v->columns.size * scale,v->rows.size * scale,&outerWidth,&outerHeight);
    if (outerWidth <= monitor.rcWork.right - monitor.rcWork.left &&
        outerHeight <= monitor.rcWork.bottom - monitor.rcWork.top) break;
    scale--;
    }
  return (scale < 1) ? 1 : scale;
  }

/* Keeps the client area from shrinking below the buffer. */
static void _rcs_limitWindowSize(rcs_Viewport v,MINMAXINFO* limits) {
  Int32 outerWidth,outerHeight;
  _rcs_frameSize(v->window,v->columns.size,v->rows.size,&outerWidth,&outerHeight);
  limits->ptMinTrackSize.x = outerWidth;
  limits->ptMinTrackSize.y = outerHeight;
  }
#endif

rcs_Viewport rcs_createViewport(rcs_Display* d,rcs_Window win,rcs_Pixmap buffer,Int32 width,Int32 height) {
  rcs_Viewport      v;
#ifdef X
  XWindowAttributes attr;
  XSizeHints*       hints;
#endif
#ifdef WIN32
  RECT              client;
  Int32             scale;
#endif
  if (width < 1 || height < 1) return NULL;
  v = (rcs_Viewport)malloc(sizeof(struct _rcs_Viewport));
  if (v == NULL) return NULL;

  v->display = d;
  v->window = win;
  v->buffer = buffer;
  v->columns.size = width;
  v->columns.scaled = width;
  v->rows.size = height;
  v->rows.scaled = height;
  _rcs_clearDirty(v);

#ifdef X
  v->gc = XCreateGC(d,win,0,NULL);
  XSetForeground(d,v->gc,BlackPixel(d,DefaultScreen(d)));
  XGetWindowAttributes(d,win,&attr);
  _rcs_layoutViewport(v,attr.width,attr.height);

  hints = XAllocSizeHints();
  if (hints != NULL) {
    hints->flags = PMinSize;
    hints->min_width = width;
    hints->min_height = height;
    XSetWMNormalHints(d,win,hints);
    XFree(hints);
    }

  if (_rcs_viewports == 0) _rcs_viewports = XUniqueContext();
  XSaveContext(d,win,_rcs_viewports,(XPointer)v);
#endif
#ifdef WIN32
  SetWindowLongPtr(win,GWLP_USERDATA,(LONG_PTR)v);
  scale = _rcs_dpiScale(v);
  _rcs_setClientSize(win,width * scale,height * scale);
  GetClientRect(win,&client);
  _rcs_layoutViewport(v,client.right,client.bottom);
#endif
  return v;
  }

/* Marks a buffer area as changed; it reaches the window at the next rcs_presentViewport(). */
void rcs_invalidateViewport(rcs_Viewport v,Int32 x,Int32 y,Int32 w,Int32 h) {
  Int32 right;
  Int32 bottom;
  right = x + w;
  bottom = y + h;
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (right > v->columns.size) right = v->columns.size;
  if (bottom > v->rows.size) bottom = v->rows.size;
  if (right <= x || bottom <= y) return;

  if (v->dirty.right <= v->dirty.left) {
    v->dirty.left = x;
    v->dirty.top = y;
    v->dirty.right = right;
    v->dirty.bottom = bottom;
    return;
    }

  if (x < v->dirty.left) v->dirty.left = x;
  if (y < v->dirty.top) v->dirty.top = y;
  if (right > v->dirty.right) v->dirty.right = right;
  if (bottom > v->dirty.bottom) v->dirty.bottom = bottom;
  }

void rcs_presentViewport(rcs_Viewport v) {
#ifdef WIN32
  HDC dc;
#endif
  if (v->dirty.right <= v->dirty.left) return;

#ifdef X
  _rcs_copyToWindow(v,v->window,v->dirty);
  XFlush(v->display);
#endif
#ifdef WIN32
  dc = GetDC(v->window);
  _rcs_copyToWindow(v,dc,v->dirty);
  ReleaseDC(v->window,dc);
#endif
  _rcs_clearDirty(v);
  }

#ifdef WIN32
rcs_Display disp;
Int32 _shiftState;
Int32 _ctrlState;
Int32 _capsState;

/* Text cell of the Win32 font: one column of the game's 8-pixel text grid */
#define FONT_WIDTH   8
#define FONT_HEIGHT  12

#include "rgb.h"

Int32 _rcs_winTranslate(Int32 key) {
        if (key >= 'A' && key <= 'Z') {
                if (_ctrlState == 1) key -= 64;
                else if ((_shiftState ^ ((GetKeyState(VK_CAPITAL))?1:0)) == 0) key+=32;
        }
        else if (key >= '0' && key <= '9') {
                if (_shiftState == 1) {
                        switch(key) {
                        case '1': key = '!'; break;
                        case '2': key = '@'; break;
                        case '3': key = '#'; break;
                        case '4': key = '$'; break;
                        case '5': key = '%'; break;
                        case '6': key = '^'; break;
                        case '7': key = '&'; break;
                        case '8': key = '*'; break;
                        case '9': key = '('; break;
                        case '0': key = ')'; break;
                        }
                }
        }
        else {
                if (_shiftState) {
                        switch(key) {
                        case 8: break;
                        case 9: break;
                        case 10: break;
                        case 13: break;
                        case 27: break;
                        case 32: break;
                        case 186: key = ':'; break;
                        case 187: key = '+'; break;
                        case 188: key = '<'; break;
                        case 189: key = '_'; break;
                        case 190: key = '>'; break;
                        case 191: key = '?'; break;
                        case 192: key = '~'; break;
                        case 219: key = '{'; break;
                        case 220: key = '|'; break;
                        case 221: key = '}'; break;
                        case 222: key = '"'; break;
                        default: key += 0xf000;
                        }
                } else {
                        switch(key) {
                        case 8: break;
                        case 9: break;
                        case 10: break;
                        case 13: break;
                        case 27: break;
                        case 32: break;
                        case 186: key = ';'; break;
                        case 187: key = '='; break;
                        case 188: key = ','; break;
                        case 189: key = '-'; break;
                        case 190: key = '.'; break;
                        case 191: key = '/'; break;
                        case 192: key = '`'; break;
                        case 219: key = '['; break;
                        case 220: key = '\\'; break;
                        case 221: key = ']'; break;
                        case 222: key = '\''; break;
                        default: key += 0xf000;
                        }
                }
        }
        return key;
}

Int32 _rcs_isBitmap(rcs_Window w) {
  Int32 i;
  if (disp.numPixmaps==0) return FALSE;
  for (i=0;i<disp.numPixmaps;i++)
    if (disp.pixmapIDs[i]==w) return TRUE;
  return FALSE;
  }

LRESULT CALLBACK rcs_WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
  rcs_Viewport viewport;
  PAINTSTRUCT  paint;
  HDC          dc;
  RECT*        suggested;
  viewport = (rcs_Viewport)GetWindowLongPtr(hWnd,GWLP_USERDATA);

  /* Painting happens here so the window also redraws inside Windows' own resize loop */
  switch (message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    case WM_DPICHANGED:
      suggested = (RECT*)lParam;
      SetWindowPos(hWnd,NULL,suggested->left,suggested->top,suggested->right - suggested->left,
                   suggested->bottom - suggested->top,SWP_NOZORDER | SWP_NOACTIVATE);
      return 0;

    case WM_GETMINMAXINFO:
      if (viewport == NULL) break;
      _rcs_limitWindowSize(viewport,(MINMAXINFO*)lParam);
      return 0;

    case WM_SIZE:
      if (viewport != NULL) _rcs_layoutViewport(viewport,LOWORD(lParam),HIWORD(lParam));
      break;

    case WM_ERASEBKGND:
      if (viewport != NULL) return 1;
      break;

    case WM_PAINT:
      if (viewport == NULL) break;
      dc = BeginPaint(hWnd,&paint);
      _rcs_paintViewport(viewport,dc);
      EndPaint(hWnd,&paint);
      return 0;
    }
  return DefWindowProc(hWnd, message, wParam, lParam);
  }

void rcs_initWin32(HINSTANCE hInstance) {
        WNDCLASSEX wcex;
  HDC        dc;
  HGDIOBJ    previous;
  TEXTMETRIC metrics;

  /* Without this, Windows stretches the window on scaled displays and blurs every pixel */
  SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

  disp.hInstance=hInstance;
  strcpy(disp.className,"rcsawlib");
  disp.numPixmaps=0;
  disp.pixmapIDs=NULL;
  disp.font=CreateFont(FONT_HEIGHT,FONT_WIDTH,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
                       OEM_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
                       NONANTIALIASED_QUALITY,FIXED_PITCH | FF_MODERN,"Terminal");

  dc = GetDC(NULL);
  previous = SelectObject(dc,disp.font);
  GetTextMetrics(dc,&metrics);
  SelectObject(dc,previous);
  ReleaseDC(NULL,dc);
  disp.fontAscent = metrics.tmAscent;
  disp.fontHeight = metrics.tmHeight;
  disp.fontOverhang = (metrics.tmMaxCharWidth > FONT_WIDTH) ? metrics.tmMaxCharWidth - FONT_WIDTH : 0;

        wcex.cbSize = sizeof(WNDCLASSEX);

        wcex.style                      = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc        = (WNDPROC)rcs_WndProc;
        wcex.cbClsExtra         = 0;
        wcex.cbWndExtra         = 0;
        wcex.hInstance          = hInstance;
        wcex.hIcon                      = NULL;
        wcex.hCursor            = LoadCursor(NULL, IDC_ARROW);
        wcex.hbrBackground      = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wcex.lpszMenuName       = NULL;
        wcex.lpszClassName      = disp.className;
        wcex.hIconSm            = NULL;

        if (!RegisterClassEx(&wcex)) {
          MessageBox(NULL, TEXT("Could not register window class."),
                     TEXT("Error"), MB_ICONERROR | MB_OK);
          }
        _shiftState = 0;
        _ctrlState = 0;
        _capsState = (GetKeyState(VK_CAPITAL)) ? 1 : 0;
  }

#endif

unsigned long rcs_RGBToPixel(Int32 red, Int32 green, Int32 blue) {
  unsigned long pixel;
  if (red_shift < 0)
    pixel = (red & red_mask) << -red_shift;
  else
    pixel = (red & red_mask) >> red_shift;
  if (green_shift < 0)
    pixel |= (green & green_mask) << -green_shift;
  else
    pixel |= (green & green_mask) >> green_shift;
  if (blue_shift < 0)
    pixel |= (blue & blue_mask) << -blue_shift;
  else
    pixel |= (blue & blue_mask) >> blue_shift;
  return pixel;
  }

/* ************************ */
/* ***** Build Colors ***** */
/* ************************ */
#ifdef X
void  _rcs_buildColors(rcs_Display* d,unsigned long* colors,const char** data,Int32 *none) {
  XColor clr1,clr2;
#endif
#ifdef WIN32
void  _rcs_buildColors(rcs_Display* d,COLORREF* colors,const char** data,Int32 *none) {
#endif
  Int32        sizeX,sizeY,numColors;
  Int32        x,i;
  char       index,temp,index2;
  char       buffer[80];
  char       nbuffer[8];
  Int32        colorIndex;
  Int32        bytesPer;
  unsigned long        red,green,blue;
  *none=-1;
  sscanf(data[0],"%d %d %d %d",&sizeX,&sizeY,&numColors,&bytesPer);
  for (x=1;x<=numColors;x++) {
    if (bytesPer != 2) {
      sscanf(data[x],"%c %c %s",&index,&temp,buffer);
      colorIndex = index;
      } else {
      sscanf(data[x],"%c%c %c %s",&index,&index2,&temp,buffer);
      colorIndex = (index-32)*96 + (index2-32);
      }
    if (buffer[0] != '#') {
      if (strcmp(buffer,"None")==0) *none=index;
#ifdef X
      XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),buffer,
                       &clr1,&clr2);
      colors[colorIndex]=clr1.pixel;
#endif
#ifdef WIN32
      rcs_namedColor(buffer,&red,&green,&blue);
      colors[colorIndex]=RGB(red,green,blue);
#endif
      } else {
      for (i=0;i<4;i++) nbuffer[i]=buffer[i+1];
      nbuffer[4]=0;
      sscanf(nbuffer,"%lx",&red);
      for (i=0;i<4;i++) nbuffer[i]=buffer[i+5];
      nbuffer[4]=0;
      sscanf(nbuffer,"%lx",&green);
      for (i=0;i<4;i++) nbuffer[i]=buffer[i+9];
      nbuffer[4]=0;
      sscanf(nbuffer,"%lx",&blue);
#ifdef WIN32
      if (red>255) red/=256;
      if (blue>255) blue/=256;
      if (green>255) green/=256;
#endif
#ifdef X
//      colors[colorIndex].blue=blue;
//      colors[colorIndex].red=red;
//      colors[colorIndex].green=green;
//      colors[colorIndex].flags=DoRed | DoGreen | DoBlue;
//      XAllocColor(d,DefaultColormap(d,DefaultScreen(d)),&colors[colorIndex]);
      colors[colorIndex] = rcs_RGBToPixel(red, green, blue);
#endif
#ifdef WIN32
      colors[colorIndex]=RGB(red,green,blue);
#endif
      }
    }
  }

/* ******************** */
/* ***** Close GC ***** */
/* ******************** */
void rcs_closeGC(rcs_Display* d,rcs_GC gc) {
#ifdef X
  XFreeGC(d,gc);
#endif
#ifdef WIN32
  free(gc);
#endif
  }

/* ********************* */
/* ***** Copy Area ***** */
/* ********************* */
void rcs_copyArea(rcs_Display* d,rcs_Window src,rcs_Window dest,rcs_GC gc,Int32 sx,Int32 sy,Int32 w,Int32 h,Int32 dx,Int32 dy) {
#ifdef X
  XCopyArea(d,src,dest,gc,sx,sy,w,h,dx,dy);
#endif
#ifdef WIN32
  HDC dc;
  HDC ddc;
  if (_rcs_isBitmap(src)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,src);
    } else dc=GetDC(src);
  if (_rcs_isBitmap(dest)==TRUE) {
    ddc=CreateCompatibleDC(0);
    SelectObject(ddc,dest);
    } else ddc=GetDC(dest);
  BitBlt(ddc,dx,dy,w,h,dc,sx,sy,SRCCOPY);
  if (_rcs_isBitmap(src)==TRUE) DeleteDC(dc);
    else ReleaseDC(src,dc);
  if (_rcs_isBitmap(dest)==TRUE) DeleteDC(ddc);
    else ReleaseDC(dest,ddc);
#endif
  }

/* ************************* */
/* ***** Create Pixmap ***** */
/* ************************* */
rcs_Pixmap rcs_createPixmap(rcs_Display* d,rcs_Window win,Int32 x,Int32 y) {
  rcs_Pixmap ret;
#ifdef X
  XWindowAttributes attr;
  XGetWindowAttributes(d,win,&attr);
  ret=XCreatePixmap(d,win,x,y,attr.depth);
#endif
#ifdef WIN32
  HBITMAP    bitmap;
  HDC        dc;
  dc=GetDC(win);
  bitmap=CreateCompatibleBitmap(dc,x,y);
  ReleaseDC(win,dc);
  ret=(rcs_Pixmap)bitmap;
  if (disp.numPixmaps == 0) disp.pixmapIDs=(rcs_Window*)malloc(sizeof(rcs_Window));
    else disp.pixmapIDs=(rcs_Window*)realloc(disp.pixmapIDs,(sizeof(rcs_Window))*(disp.numPixmaps+1));
  disp.pixmapIDs[disp.numPixmaps++]=ret;
#endif
  return ret;
  }

/* ************************* */
/* ***** Create Window ***** */
/* ************************* */
rcs_Window  rcs_createWindow(rcs_Display* d,rcs_Window parent,Int32 x,Int32 y,Int32 width,Int32 height) {
  rcs_Window ret;
#ifdef X
  unsigned long mask;
  XSetWindowAttributes attr;
  attr.background_pixel=BlackPixel(d,DefaultScreen(d));
  mask=CWBackPixel;
  ret=XCreateWindow(d,parent,x,y,width,height,0,
                       CopyFromParent,
                       CopyFromParent,
                       CopyFromParent,
                       mask,&attr);
  XSelectInput(d,ret,ExposureMask | ButtonPressMask | ButtonReleaseMask |
               ButtonMotionMask | KeyPressMask | KeyReleaseMask |
               StructureNotifyMask | FocusChangeMask | EnterWindowMask |
               LeaveWindowMask | PointerMotionMask);
  ShiftState = 0;
  AltState = 0;
#endif
#ifdef WIN32

  if (parent == GetDesktopWindow()) {
    ret = CreateWindow(d->className, "rcsawlib", WS_OVERLAPPEDWINDOW,
       x, y, width, height, NULL, NULL, d->hInstance, NULL);
    if (ret) _rcs_setClientSize(ret,width,height);
    }
  else {
    ret = CreateWindow(d->className, "rcsawlib", WS_CHILD,
       x, y, width, height, parent, NULL, d->hInstance, NULL);
    }
  if (!ret) MessageBox(NULL, TEXT("Hmmmmm"), TEXT("Error"), MB_ICONERROR | MB_OK);

#endif
  return ret;
  }

/* *************************** */
/* ***** Draw Filled Box ***** */
/* *************************** */
void rcs_drawFilledBox(rcs_Display* d,rcs_Window win,rcs_GC gc,
                  Int32 x,Int32 y,Int32 w,Int32 h) {
#ifdef X
  XFillRectangle(d,win,gc,x,y,w,h);
#endif
#ifdef WIN32
  HDC dc;
  HPEN pen;
  HBRUSH brush;
  RECT rect;
  rect.top=y;
  rect.left=x;
  rect.bottom=y+h;
  rect.right=x+w;
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
  pen=CreatePen(PS_SOLID,0,gc->foreground);
  brush=CreateSolidBrush(gc->foreground);
  pen=(HPEN)SelectObject(dc,pen);
  FillRect(dc,&rect,brush);
  pen=(HPEN)SelectObject(dc,pen);
  DeleteObject(pen);
  DeleteObject(brush);
  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
#endif
  }

/* ********************* */
/* ***** Draw Line ***** */
/* ********************* */
void rcs_drawLine(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x1,Int32 y1,Int32 x2,Int32 y2) {
#ifdef X
  XDrawLine(d,win,gc,x1,y1,x2,y2);
#endif
#ifdef WIN32
  HDC dc;
  HPEN pen;
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
  pen=CreatePen(PS_SOLID,0,gc->foreground);
  pen=(HPEN)SelectObject(dc,pen);
  MoveToEx(dc,x1,y1,NULL);
  LineTo(dc,x2,y2);
  pen=(HPEN)SelectObject(dc,pen);
  DeleteObject(pen);
  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
#endif
  }

/* *********************** */
/* ***** Draw Pixmap ***** */
/* *********************** */
void  rcs_drawPixmap(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 dx,Int32 dy,const char** data) {
  Int32        sizeX,sizeY,numColors;
  Int32        x,y;
/*  char       index; */
  Int32        none;
  Int32        colorIndex;
  Int32        bytesPer;
#ifdef X
  unsigned long colors[10000];
#endif
#ifdef WIN32
  HDC        dc;
  HPEN       pen,oldPen;
  COLORREF   colors[10000];
#endif
  none=-1;
  _rcs_buildColors(d,colors,data,&none);
  sscanf(data[0],"%d %d %d %d",&sizeX,&sizeY,&numColors,&bytesPer);
#ifdef WIN32
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
#endif
  for (y=0;y<sizeY;y++)
    for (x=0;x<sizeX;x++) {
      if (bytesPer != 2) colorIndex=data[y+numColors+1][x];
        else colorIndex = (data[y+numColors+1][x*2]-32) * 96 +
                          (data[y+numColors+1][x*2+1]-32);
      if (colorIndex != none) {
#ifdef X
        XSetForeground(d,gc,colors[colorIndex]);
        XDrawPoint(d,win,gc,x+dx,y+dy);
#endif
#ifdef WIN32
        pen=CreatePen(PS_SOLID,0,colors[colorIndex]);
        oldPen=(HPEN)SelectObject(dc,pen);
        MoveToEx(dc,x+dx,y+dy,NULL);
        LineTo(dc,x+1+dx,y+dy);
        oldPen=(HPEN)SelectObject(dc,oldPen);
        DeleteObject(oldPen);
#endif
        }
      }
#ifdef WIN32
  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
#endif    
  }

/* ********************** */
/* ***** Draw Point ***** */
/* ********************** */
void rcs_drawPoint(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x,Int32 y) {
#ifdef X
  XDrawPoint(d,win,gc,x,y);
#endif
#ifdef WIN32
  HDC dc;
  HPEN pen;
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
  pen=CreatePen(PS_SOLID,0,gc->foreground);
  pen=(HPEN)SelectObject(dc,pen);
  MoveToEx(dc,x,y,NULL);
  LineTo(dc,x+1,y);
  pen=(HPEN)SelectObject(dc,pen);
  DeleteObject(pen);
  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
#endif
  }

/* *********************** */
/* ***** Draw String ***** */
/* *********************** */
void rcs_drawString(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x,Int32 y,char* buffer,Int32 clr) {
#ifdef X
  if (clr) XDrawImageString(d,win,gc,x,y,buffer,strlen(buffer));
  else     XDrawString(d,win,gc,x,y,buffer,strlen(buffer));
#endif
#ifdef WIN32
  HDC   dc;
  Int32 i;
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
  SelectObject(dc,gc->font);
  SetTextAlign(dc,TA_LEFT | TA_BASELINE);
  SetBkColor(dc,gc->background);
  SetTextColor(dc,gc->foreground);
  SetBkMode(dc,TRANSPARENT);

  /* One glyph per cell keeps columns aligned even if Windows substitutes the font */
  for (i=0; buffer[i] != 0; i++)
    TextOut(dc,x + i * FONT_WIDTH,y,&buffer[i],1);

  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
#endif
  }

/* ************************* */
/* ***** String Bounds ***** */
/* ************************* */
/* The rectangle rcs_drawString() may touch for the same arguments, with or without clr. */
void rcs_stringBounds(rcs_Display* d,rcs_GC gc,Int32 x,Int32 y,char* buffer,
                      Int32* left,Int32* top,Int32* width,Int32* height) {
  Int32 length;
#ifdef X
  /* No GC is ever given its own font, so they all share the server default */
  static XFontStruct* font = NULL;
  XCharStruct overall;
  Int32       direction,ascent,descent;
  Int32       inkLeft,inkRight,above,below;
#endif
  length = strlen(buffer);

#ifdef X
  if (font == NULL) font = XQueryFont(d,XGContextFromGC(gc));
  if (font == NULL) {
    *left = x;
    *top = y;
    *width = 0;
    *height = 0;
    return;
    }

  XTextExtents(font,buffer,length,&direction,&ascent,&descent,&overall);
  inkLeft = (overall.lbearing < 0) ? overall.lbearing : 0;
  inkRight = (overall.rbearing > overall.width) ? overall.rbearing : overall.width;
  above = (overall.ascent > ascent) ? overall.ascent : ascent;
  below = (overall.descent > descent) ? overall.descent : descent;

  *left = x + inkLeft;
  *top = y - above;
  *width = inkRight - inkLeft;
  *height = above + below;
#endif
#ifdef WIN32
  *left = x;
  *top = y - d->fontAscent;
  *width = (length > 0) ? length * FONT_WIDTH + d->fontOverhang : 0;
  *height = d->fontHeight;
#endif
  }

/* ***************** */
/* ***** Flush ***** */
/* ***************** */
void rcs_Flush(rcs_Display* d) {
#ifdef X
  XFlush(d);
#endif
  }

/* ********************* */
/* ***** Get Event ***** */
/* ********************* */
rcs_Event rcs_getEvent(rcs_Display* d) {
  rcs_Event ret;
#ifdef X
  XEvent event;
  KeySym key;
  XComposeStatus status;
  char tmp[10];
  XNextEvent(d,&event);
  switch (event.type) {
    case FocusIn:     ret.type=EVENT_FOCUSIN;
                      ret.d3=event.xcrossing.mode;
                      ret.d4=event.xcrossing.focus;
                      ret.window=event.xcrossing.window;
                      break;
    case FocusOut:    ret.type=EVENT_FOCUSOUT;
                      ret.d3=event.xcrossing.mode;
                      ret.d4=event.xcrossing.focus;
                      ret.window=event.xcrossing.window;
                      break;
    case EnterNotify: ret.type=EVENT_ENTER;
                      ret.d1=event.xcrossing.x;
                      ret.d2=event.xcrossing.y;
                      ret.d3=event.xcrossing.mode;
                      ret.d4=event.xcrossing.focus;
                      ret.window=event.xcrossing.window;
                      break;
    case LeaveNotify: ret.type=EVENT_LEAVE;
                      ret.d1=event.xcrossing.x;
                      ret.d2=event.xcrossing.y;
                      ret.d3=event.xcrossing.mode;
                      ret.d4=event.xcrossing.focus;
                      ret.window=event.xcrossing.window;
                      break;
    case ConfigureNotify:
                      ret.type=EVENT_CONFIGURE;
                      ret.d1=event.xconfigure.x;
                      ret.d2=event.xconfigure.y;
                      ret.d3=event.xconfigure.width;
                      ret.d4=event.xconfigure.height;
                      ret.window=event.xconfigure.window;
                      break;
    case MotionNotify:ret.type=EVENT_MOTION;
                      ret.d1=event.xmotion.state;
                      ret.d2=event.xmotion.x;
                      ret.d3=event.xmotion.y;
                      ret.window=event.xmotion.window;
                      break;
    case ButtonPress:ret.type=EVENT_BUTTON_PRESS;
                     ret.window=event.xbutton.window;
                     ret.d1=event.xbutton.button;
                     ret.d2=event.xbutton.x;
                     ret.d3=event.xbutton.y;
                     break;
    case ButtonRelease:ret.type=EVENT_BUTTON_RELEASE;
                     ret.window=event.xbutton.window;
                     ret.d1=event.xbutton.button;
                     ret.d2=event.xbutton.x;
                     ret.d3=event.xbutton.y;
                     break;
    case Expose:ret.type=EVENT_EXPOSE;
                ret.window=event.xexpose.window;
                ret.d1=event.xexpose.x;
                ret.d2=event.xexpose.y;
                ret.d3=event.xexpose.width;
                ret.d4=event.xexpose.height;
                ret.d5=event.xexpose.count;
                break;
    case KeyRelease:ret.type=EVENT_KEY_RELEASE;
                  ret.window=event.xkey.window;
                  XLookupString((XKeyEvent*)&event,tmp,10,&key,&status);
                  ret.d1=key;
                  switch (ret.d1) {
                    case XK_Return:ret.d1=13; break;
                    case XK_Escape:ret.d1=27; break;
                    case XK_Shift_L: ShiftState = 0; break;
                    case XK_Shift_R: ShiftState = 0; break;
                    case XK_Alt_L: AltState = 0; break;
                    case XK_Alt_R: AltState = 0; break;
                    case XK_Control_L: CtrlState = 0; ret.d1 = 0; break;
                    case XK_Control_R: CtrlState = 0; ret.d1 = 0; break;
                    case XK_F1:
                    case XK_F2:
                    case XK_F3:
                    case XK_F4:
                    case XK_F5:
                    case XK_F6:
                    case XK_F7:
                    case XK_F8:
                    case XK_F9:
                    case XK_F10:if (ShiftState) ret.d1 |= 0x10000;
                                if (AltState) ret.d1 |= 0x20000;
                                if (CtrlState) ret.d1 |= 0x40000;
                    }
                  break;
    case KeyPress:ret.type=EVENT_KEY_PRESS;
                  ret.window=event.xkey.window;
                  XLookupString((XKeyEvent*)&event,tmp,10,&key,&status);
                  ret.d1=key;
                  switch (ret.d1) {
                    case XK_Return:ret.d1=13; break;
                    case XK_Escape:ret.d1=27; break;
                    case XK_Shift_L: ShiftState = -1; break;
                    case XK_Shift_R: ShiftState = -1; break;
                    case XK_Alt_L: AltState = -1; break;
                    case XK_Alt_R: AltState = -1; break;
                    case XK_Control_L: CtrlState = -1; break;
                    case XK_Control_R: CtrlState = -1; break;
                    case XK_F1:
                    case XK_F2:
                    case XK_F3:
                    case XK_F4:
                    case XK_F5:
                    case XK_F6:
                    case XK_F7:
                    case XK_F8:
                    case XK_F9:
                    case XK_F10:if (ShiftState) ret.d1 |= 0x10000;
                                if (AltState) ret.d1 |= 0x20000;
                                if (CtrlState) ret.d1 |= 0x40000;
                               break;
                    }
                  if (CtrlState && ret.d1 >= XK_a && ret.d1 <= XK_z) ret.d1 -= XK_a - 1;
                  if (CtrlState && ret.d1 >= XK_A && ret.d1 <= XK_Z) ret.d1 -= XK_A - 1;
                  break;
    }
  _rcs_viewportEvent(d,&event);
#endif
#ifdef WIN32
  MSG msg;
  RECT rect;

  ret.type=EVENT_NONE;
  if (GetMessage(&msg, NULL, 0, 0) == 0) {
	  ret.type = EVENT_QUIT;
	  return ret;
  }
  if (msg.message == WM_SYSCOMMAND && msg.wParam == SC_CLOSE) {
	  ret.type = EVENT_CLOSE;
      ret.window = msg.hwnd;
	  return ret;
  }
  if (msg.message == WM_DESTROY) {
	  ret.type = EVENT_CLOSE;
      ret.window = msg.hwnd;
	  return ret;
  }
  if (msg.message == WM_CLOSE) {
	  ret.type = EVENT_CLOSE;
      ret.window = msg.hwnd;
	  return ret;
  }
  if (msg.message == WM_MOVE) {
    ret.type = EVENT_CONFIGURE;
	ret.window = msg.hwnd;
    ret.d1 = LOWORD(msg.lParam);
	ret.d2 = HIWORD(msg.lParam);
	ret.d3 = 0;
	ret.d4 = 0;
  }
    if (msg.message == WM_MOUSEMOVE) {
      ret.type=EVENT_MOTION;
      ret.window=msg.hwnd;
      ret.d1=1;
      ret.d2=msg.lParam & 0xffff;
      ret.d3=msg.lParam >> 16;
      }
    if (msg.message == WM_LBUTTONDOWN) {
      ret.type=EVENT_BUTTON_PRESS;
      ret.window=msg.hwnd;
      ret.d1=1;
      ret.d2=msg.lParam & 0xffff;
      ret.d3=msg.lParam >> 16;
      }
    else if (msg.message == WM_RBUTTONDOWN) {
      ret.type=EVENT_BUTTON_PRESS;
      ret.window=msg.hwnd;
      ret.d1=2;
      ret.d2=msg.lParam & 0xffff;
      ret.d3=msg.lParam >> 16;
      }
    else if (msg.message == WM_LBUTTONUP) {
      ret.type=EVENT_BUTTON_RELEASE;
      ret.window=msg.hwnd;
      ret.d1=1;
      ret.d2=msg.lParam & 0xffff;
      ret.d3=msg.lParam >> 16;
      }
    else if (msg.message == WM_RBUTTONUP) {
      ret.type=EVENT_BUTTON_RELEASE;
      ret.window=msg.hwnd;
      ret.d1=2;
      ret.d2=msg.lParam & 0xffff;
      ret.d3=msg.lParam >> 16;
      }
    else if (msg.message == WM_PAINT) {
      ret.type=EVENT_EXPOSE;
      ret.window=msg.hwnd;
      GetUpdateRect(ret.window,&rect,FALSE);
      ret.d1=rect.left;
      ret.d2=rect.top;
      ret.d3=rect.right-rect.left;
      ret.d4=rect.bottom-rect.top;
      ret.d5=0;
      }
    if (msg.message == WM_KEYDOWN) {
	  ret.type=EVENT_KEY_PRESS;
      ret.window=msg.hwnd;
      ret.d1=msg.wParam;
	  if (ret.d1 == VK_SHIFT) {
		  _shiftState = 1;
	  }
	  if (ret.d1 == VK_CONTROL) {
		  _ctrlState = 1;
	  }
	  if (ret.d1 == VK_CAPITAL) {
		  _capsState ^= 1;
	  }
      ret.d1 = _rcs_winTranslate(ret.d1);
	}
    if (msg.message == WM_KEYUP) {
	  ret.type=EVENT_KEY_RELEASE;
      ret.window=msg.hwnd;
      ret.d1=msg.wParam;
	  if (ret.d1 == VK_SHIFT) {
		  _shiftState = 0;
	  }
	  if (ret.d1 == VK_CONTROL) {
		  _ctrlState = 0;
	  }
      ret.d1 = _rcs_winTranslate(ret.d1);
	}
/*
	TranslateMessage(&msg);

    if (msg.message == WM_CHAR) {
      ret.type=EVENT_KEY_PRESS;
      ret.window=msg.hwnd;
      ret.d1=msg.wParam;
      }
*/
		DispatchMessage(&msg);
      
#endif
  return ret;
  }

/* **************************** */
/* ***** Named Background ***** */
/* **************************** */
void rcs_namedBackground(rcs_Display* d,rcs_GC gc,const char* color) {
#ifdef X
  XColor clr,exact;
  XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),color,
                   &clr,&exact);
  XSetBackground(d,gc,clr.pixel);
#endif
#ifdef WIN32
  Int32 red,green,blue;
  rcs_namedColor(color,&red,&green,&blue);
  gc->background=RGB(red,green,blue);
#endif
  }

/* **************************** */
/* ***** Named Foreground ***** */
/* **************************** */
void rcs_namedForeground(rcs_Display* d,rcs_GC gc,const char* color) {
#ifdef X
  XColor clr,exact;
  XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),color,
                   &clr,&exact);
  XSetForeground(d,gc,clr.pixel);
#endif
#ifdef WIN32
  Int32 red,green,blue;
  rcs_namedColor(color,&red,&green,&blue);
  gc->foreground=RGB(red,green,blue);
#endif
  }

/* ************************ */
/* ***** Open Display ***** */
/* ************************ */
rcs_Display *rcs_openDisplay(char* display_name) {
#ifdef X
  return XOpenDisplay(display_name);
#endif
#ifdef WIN32
  return &disp;
#endif
  }

/* ******************* */
/* ***** Open GC ***** */
/* ******************* */
rcs_GC rcs_openGC(rcs_Display* d,rcs_Window win) {
#ifdef X
  unsigned long mask;
  XGCValues     values;
  mask=0;
  mask=GCForeground | GCBackground;
  values.foreground=BlackPixel(d,DefaultScreen(d));
  values.background=WhitePixel(d,DefaultScreen(d));
  return XCreateGC(d,win,mask,&values);
#endif
#ifdef WIN32
  rcs_GC ret;
        ret=(rcs_GC)malloc(sizeof(struct _GC));
  ret->foreground=RGB(0,0,0);
  ret->background=RGB(255,255,255);
  ret->font=disp.font;
  return ret;
#endif
  }

/* ********************** */
/* ***** Poll Event ***** */
/* ********************** */
rcs_Event rcs_pollEvent(rcs_Display* d) {
  rcs_Event ret;
#ifdef X
  if (XPending(d)>0) return rcs_getEvent(d);
#endif
#ifdef WIN32
  MSG msg;
  if (PeekMessage(&msg, NULL, 0, 0,PM_NOREMOVE)!=0) return rcs_getEvent(d);
#endif
  ret.type=EVENT_NONE;
  return ret;
  }

void  rcs_getDisplaySize(rcs_Display* d,Int32* width,Int32* height) {
#ifdef X
  *height = XDisplayHeight(d,DefaultScreen(d));
  *width = XDisplayWidth(d,DefaultScreen(d));
#endif
  }

/* *********************** */
/* ***** Root Window ***** */
/* *********************** */
rcs_Window rcs_rootWindow(rcs_Display* d) {
#ifdef X
  return DefaultRootWindow(d);
#endif
#ifdef WIN32
  return GetDesktopWindow();
#endif
  }

/* *********************** */
/* ***** Save Pixmap ***** */
/* *********************** */
/* Fills pixels with bottom-up rows of 24-bit BGR, the pixel layout of a BMP file. */
static Int32 _rcs_readPixmap(rcs_Display* d,rcs_Pixmap pixmap,Int32 width,Int32 height,
                             UInt32 rowSize,unsigned char* pixels) {
#ifdef X
  XImage*        image;
  unsigned long  pixel;
  unsigned char* out;
  Int32          x,y;
  image = XGetImage(d,pixmap,0,0,width,height,AllPlanes,ZPixmap);
  if (image == NULL) return 0;

  for (y=0; y<height; y++) {
    out = pixels + (height-1-y) * rowSize;
    for (x=0; x<width; x++) {
      pixel = XGetPixel(image,x,y);
      *out++ = _rcs_pixelChannel(pixel,blue_mask,blue_shift);
      *out++ = _rcs_pixelChannel(pixel,green_mask,green_shift);
      *out++ = _rcs_pixelChannel(pixel,red_mask,red_shift);
      }
    }

  XDestroyImage(image);
  return 1;
#endif
#ifdef WIN32
  BITMAPINFO info;
  HDC        dc;
  Int32      lines;
  memset(&info,0,sizeof(info));
  info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  info.bmiHeader.biWidth = width;
  info.bmiHeader.biHeight = height;
  info.bmiHeader.biPlanes = 1;
  info.bmiHeader.biBitCount = 24;
  info.bmiHeader.biCompression = BI_RGB;

  dc = GetDC(NULL);
  lines = GetDIBits(dc,(HBITMAP)pixmap,0,height,pixels,&info,DIB_RGB_COLORS);
  ReleaseDC(NULL,dc);
  return lines == height;
#endif
  }

static void _rcs_writeShort(FILE* file,UInt32 value) {
  fputc(value & 0xff,file);
  fputc((value >> 8) & 0xff,file);
  }

static void _rcs_writeLong(FILE* file,UInt32 value) {
  _rcs_writeShort(file,value & 0xffff);
  _rcs_writeShort(file,value >> 16);
  }

static Int32 _rcs_writeBMP(char* filename,Int32 width,Int32 height,UInt32 rowSize,unsigned char* pixels) {
  FILE*  file;
  UInt32 imageSize;
  UInt32 written;
  imageSize = rowSize * height;
  file = fopen(filename,"wb");
  if (file == NULL) return 0;

  /* BITMAPFILEHEADER (14 bytes); the pixels follow both headers */
  fputc('B',file);
  fputc('M',file);
  _rcs_writeLong(file,14 + 40 + imageSize);
  _rcs_writeLong(file,0);
  _rcs_writeLong(file,14 + 40);

  /* BITMAPINFOHEADER (40 bytes): 24 bits per pixel, uncompressed, 72 dpi */
  _rcs_writeLong(file,40);
  _rcs_writeLong(file,width);
  _rcs_writeLong(file,height);
  _rcs_writeShort(file,1);
  _rcs_writeShort(file,24);
  _rcs_writeLong(file,0);
  _rcs_writeLong(file,imageSize);
  _rcs_writeLong(file,2835);
  _rcs_writeLong(file,2835);
  _rcs_writeLong(file,0);
  _rcs_writeLong(file,0);

  written = fwrite(pixels,1,imageSize,file);
  if (fclose(file) != 0) return 0;
  return written == imageSize;
  }

Int32 rcs_savePixmap(rcs_Display* d,rcs_Pixmap pixmap,Int32 width,Int32 height,char* filename) {
  UInt32         rowSize;
  unsigned char* pixels;
  Int32          saved;
  rowSize = (width * 3 + 3) & ~3;   /* BMP rows are padded to 4 bytes */
  pixels = (unsigned char*)calloc(rowSize,height);
  if (pixels == NULL) return 0;

  saved = _rcs_readPixmap(d,pixmap,width,height,rowSize,pixels);
  if (saved) saved = _rcs_writeBMP(filename,width,height,rowSize,pixels);

  free(pixels);
  return saved;
  }

/* *************************** */
/* ***** Set Window Name ***** */
/* *************************** */
void rcs_setWindowName(rcs_Display* d,rcs_Window w,char* name) {
#ifdef X
  XTextProperty text;
  XStringListToTextProperty(&name,1,&text);
  XSetWMName(d,w,&text);
#endif
#ifdef WIN32
  SetWindowText(w,name);
#endif
  }

/* *********************** */
/* ***** Show Window ***** */
/* *********************** */
void rcs_showWindow(rcs_Display* d,rcs_Window win) {
#ifdef X
  XMapWindow(d,win);
  XFlush(d);
#endif
#ifdef WIN32
  ShowWindow(win,SW_SHOW);
  UpdateWindow(win);
#endif
  }

#endif

