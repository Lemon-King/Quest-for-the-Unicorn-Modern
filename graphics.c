#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "graphics.h"

typedef int32_t  Int32;
typedef uint32_t UInt32;

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
  };

/* What the window is painted through: a device context */
typedef HDC _rcs_Surface;

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
  RECT rect;
  if (w <= 0 || h <= 0) return;

  rect.left = x;
  rect.top = y;
  rect.right = x + w;
  rect.bottom = y + h;
  FillRect(surface,&rect,(HBRUSH)GetStockObject(BLACK_BRUSH));
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

/* Reads the image's area of the buffer into its pixels. */
static Int32 _rcs_readBuffer(rcs_Viewport v,_rcs_Image* image) {
  Int32      width;
  Int32      height;
  BITMAPINFO info;
  HBITMAP    section;
  void*      bits;
  HDC        source;
  HDC        target;
  HGDIOBJ    previousSource;
  HGDIOBJ    previousTarget;
  width = _rcs_rectWidth(image->area);
  height = _rcs_rectHeight(image->area);

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
  return 1;
  }

/* Draws the image at its place in the window. */
static void _rcs_writeWindow(rcs_Viewport v,_rcs_Surface surface,_rcs_Image* image) {
  Int32      width;
  Int32      height;
  BITMAPINFO info;
  width = _rcs_rectWidth(image->area);
  height = _rcs_rectHeight(image->area);

  _rcs_imageInfo(&info,image->area);
  SetDIBitsToDevice(surface,v->left + image->area.left,v->top + image->area.top,width,height,
                    0,0,0,height,image->pixels,&info,DIB_RGB_COLORS);
  }

/* At 1:1 the buffer is copied as it is. */
static void _rcs_copyUnscaled(rcs_Viewport v,_rcs_Surface surface,_rcs_Rect area) {
  HDC     source;
  HGDIOBJ previous;
  source = CreateCompatibleDC(surface);
  previous = SelectObject(source,v->buffer);
  BitBlt(surface,v->left + area.left,v->top + area.top,_rcs_rectWidth(area),_rcs_rectHeight(area),
         source,area.left,area.top,SRCCOPY);
  SelectObject(source,previous);
  DeleteDC(source);
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

rcs_Viewport rcs_createViewport(rcs_Display* d,rcs_Window win,rcs_Pixmap buffer,Int32 width,Int32 height) {
  rcs_Viewport      v;
  RECT              client;
  Int32             scale;
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

  SetWindowLongPtr(win,GWLP_USERDATA,(LONG_PTR)v);
  scale = _rcs_dpiScale(v);
  _rcs_setClientSize(win,width * scale,height * scale);
  GetClientRect(win,&client);
  _rcs_layoutViewport(v,client.right,client.bottom);
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
  HDC dc;
  if (v->dirty.right <= v->dirty.left) return;

  dc = GetDC(v->window);
  _rcs_copyToWindow(v,dc,v->dirty);
  ReleaseDC(v->window,dc);
  _rcs_clearDirty(v);
  }

rcs_Display disp;
Int32 _shiftState;
Int32 _ctrlState;
Int32 _capsState;

/* Text cell of the Win32 font: one column of the game's 8-pixel text grid */
#define FONT_WIDTH   8
#define FONT_HEIGHT  12

#include "rgb.h"

Int32 _rcs_winTranslate(Int32 key) {
        /* Ctrl+M would become 13, the same as Enter */
        if (key == 'M' && _ctrlState == 1) return KEY_CTRL_M;
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


/* ************************ */
/* ***** Build Colors ***** */
/* ************************ */
void  _rcs_buildColors(rcs_Display* d,COLORREF* colors,const char** data,Int32 *none) {
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
      rcs_namedColor(buffer,&red,&green,&blue);
      colors[colorIndex]=RGB(red,green,blue);
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
      if (red>255) red/=256;
      if (blue>255) blue/=256;
      if (green>255) green/=256;
      colors[colorIndex]=RGB(red,green,blue);
      }
    }
  }

/* ******************** */
/* ***** Close GC ***** */
/* ******************** */
void rcs_closeGC(rcs_Display* d,rcs_GC gc) {
  free(gc);
  }

/* ********************* */
/* ***** Copy Area ***** */
/* ********************* */
void rcs_copyArea(rcs_Display* d,rcs_Window src,rcs_Window dest,rcs_GC gc,Int32 sx,Int32 sy,Int32 w,Int32 h,Int32 dx,Int32 dy) {
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
  }

/* ************************* */
/* ***** Create Pixmap ***** */
/* ************************* */
rcs_Pixmap rcs_createPixmap(rcs_Display* d,rcs_Window win,Int32 x,Int32 y) {
  rcs_Pixmap ret;
  HBITMAP    bitmap;
  HDC        dc;
  dc=GetDC(win);
  bitmap=CreateCompatibleBitmap(dc,x,y);
  ReleaseDC(win,dc);
  ret=(rcs_Pixmap)bitmap;
  if (disp.numPixmaps == 0) disp.pixmapIDs=(rcs_Window*)malloc(sizeof(rcs_Window));
    else disp.pixmapIDs=(rcs_Window*)realloc(disp.pixmapIDs,(sizeof(rcs_Window))*(disp.numPixmaps+1));
  disp.pixmapIDs[disp.numPixmaps++]=ret;
  return ret;
  }

/* ************************* */
/* ***** Create Window ***** */
/* ************************* */
rcs_Window  rcs_createWindow(rcs_Display* d,rcs_Window parent,Int32 x,Int32 y,Int32 width,Int32 height) {
  rcs_Window ret;

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

  return ret;
  }

/* *************************** */
/* ***** Draw Filled Box ***** */
/* *************************** */
void rcs_drawFilledBox(rcs_Display* d,rcs_Window win,rcs_GC gc,
                  Int32 x,Int32 y,Int32 w,Int32 h) {
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
  }

/* ********************* */
/* ***** Draw Line ***** */
/* ********************* */
void rcs_drawLine(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x1,Int32 y1,Int32 x2,Int32 y2) {
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
  HDC        dc;
  HPEN       pen,oldPen;
  COLORREF   colors[10000];
  none=-1;
  _rcs_buildColors(d,colors,data,&none);
  sscanf(data[0],"%d %d %d %d",&sizeX,&sizeY,&numColors,&bytesPer);
  if (_rcs_isBitmap(win)==TRUE) {
    dc=CreateCompatibleDC(0);
    SelectObject(dc,win);
    } else dc=GetDC(win);
  for (y=0;y<sizeY;y++)
    for (x=0;x<sizeX;x++) {
      if (bytesPer != 2) colorIndex=data[y+numColors+1][x];
        else colorIndex = (data[y+numColors+1][x*2]-32) * 96 +
                          (data[y+numColors+1][x*2+1]-32);
      if (colorIndex != none) {
        pen=CreatePen(PS_SOLID,0,colors[colorIndex]);
        oldPen=(HPEN)SelectObject(dc,pen);
        MoveToEx(dc,x+dx,y+dy,NULL);
        LineTo(dc,x+1+dx,y+dy);
        oldPen=(HPEN)SelectObject(dc,oldPen);
        DeleteObject(oldPen);
        }
      }
  if (_rcs_isBitmap(win)==TRUE) DeleteDC(dc);
    else ReleaseDC(win,dc);
  }

/* ********************** */
/* ***** Draw Point ***** */
/* ********************** */
void rcs_drawPoint(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x,Int32 y) {
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
  }

/* *********************** */
/* ***** Draw String ***** */
/* *********************** */
void rcs_drawString(rcs_Display* d,rcs_Window win,rcs_GC gc,Int32 x,Int32 y,char* buffer,Int32 clr) {
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
  }

/* ************************* */
/* ***** String Bounds ***** */
/* ************************* */
/* The rectangle rcs_drawString() may touch for the same arguments, with or without clr. */
void rcs_stringBounds(rcs_Display* d,rcs_GC gc,Int32 x,Int32 y,char* buffer,
                      Int32* left,Int32* top,Int32* width,Int32* height) {
  Int32 length;
  length = strlen(buffer);

  *left = x;
  *top = y - d->fontAscent;
  *width = (length > 0) ? length * FONT_WIDTH + d->fontOverhang : 0;
  *height = d->fontHeight;
  }

/* ********************* */
/* ***** Get Event ***** */
/* ********************* */
rcs_Event rcs_getEvent(rcs_Display* d) {
  rcs_Event ret;
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
      
  return ret;
  }

/* **************************** */
/* ***** Named Background ***** */
/* **************************** */
void rcs_namedBackground(rcs_Display* d,rcs_GC gc,const char* color) {
  Int32 red,green,blue;
  rcs_namedColor(color,&red,&green,&blue);
  gc->background=RGB(red,green,blue);
  }

/* **************************** */
/* ***** Named Foreground ***** */
/* **************************** */
void rcs_namedForeground(rcs_Display* d,rcs_GC gc,const char* color) {
  Int32 red,green,blue;
  rcs_namedColor(color,&red,&green,&blue);
  gc->foreground=RGB(red,green,blue);
  }

/* ************************ */
/* ***** Open Display ***** */
/* ************************ */
rcs_Display *rcs_openDisplay(char* display_name) {
  return &disp;
  }

/* ******************* */
/* ***** Open GC ***** */
/* ******************* */
rcs_GC rcs_openGC(rcs_Display* d,rcs_Window win) {
  rcs_GC ret;
        ret=(rcs_GC)malloc(sizeof(struct _GC));
  ret->foreground=RGB(0,0,0);
  ret->background=RGB(255,255,255);
  ret->font=disp.font;
  return ret;
  }

/* ********************** */
/* ***** Poll Event ***** */
/* ********************** */
rcs_Event rcs_pollEvent(rcs_Display* d) {
  rcs_Event ret;
  MSG msg;
  if (PeekMessage(&msg, NULL, 0, 0,PM_NOREMOVE)!=0) return rcs_getEvent(d);
  ret.type=EVENT_NONE;
  return ret;
  }

/* *********************** */
/* ***** Root Window ***** */
/* *********************** */
rcs_Window rcs_rootWindow(rcs_Display* d) {
  return GetDesktopWindow();
  }

/* *********************** */
/* ***** Save Pixmap ***** */
/* *********************** */
/* Fills pixels with bottom-up rows of 24-bit BGR, the pixel layout of a BMP file. */
static Int32 _rcs_readPixmap(rcs_Display* d,rcs_Pixmap pixmap,Int32 width,Int32 height,
                             UInt32 rowSize,unsigned char* pixels) {
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
  SetWindowText(w,name);
  }

/* *********************** */
/* ***** Show Window ***** */
/* *********************** */
void rcs_showWindow(rcs_Display* d,rcs_Window win) {
  ShowWindow(win,SW_SHOW);
  UpdateWindow(win);
  }


