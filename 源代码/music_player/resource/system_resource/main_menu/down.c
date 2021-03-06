/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*                                                                    *
* C-file generated by                                                *
*                                                                    *
*        Bitmap Converter for emWin V5.26a.                          *
*        Compiled Aug 20 2014, 15:23:27                              *
*                                                                    *
*        (c) 1998 - 2014 Segger Microcontroller GmbH & Co. KG        *
*                                                                    *
**********************************************************************
*                                                                    *
* Source file: down                                                  *
* Dimensions:  11 * 8                                                *
* NumColors:   32bpp: 16777216 + 256                                 *
*                                                                    *
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

extern GUI_CONST_STORAGE GUI_BITMAP g_down;

static GUI_CONST_STORAGE unsigned long g_res_down[] = {
  0xEFFFFFFF, 0x46FFFFFF, 0xC3FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x57FFFFFF, 0xA3FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x0BFFFFFF, 0x9DFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x00FFFFFF, 0x03FFFFFF, 0x7DFFFFFF, 0xFBFFFFFF, 0xFFFFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x59FFFFFF, 0xF1FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x3CFFFFFF, 0xE1FFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x49FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x1CFFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x40FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x13FFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x3FFFFFFF, 0xE3FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x27FFFFFF, 0xCEFFFFFF,
  0xD2FFFFFF, 0x00FFFFFF, 0x03FFFFFF, 0x80FFFFFF, 0xFCFFFFFF, 0xFFFFFFFF, 0x00FFFFFF, 0x00FFFFFF, 0x60FFFFFF, 0xF3FFFFFF, 0xFFFFFFFF,
  0xEAFFFFFF, 0x28FFFFFF, 0xAEFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x3AFFFFFF, 0x87FFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF
};

GUI_CONST_STORAGE GUI_BITMAP g_down = {
  11, // xSize
  8, // ySize
  44, // BytesPerLine
  32, // BitsPerPixel
  (unsigned char *)g_res_down,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMP8888
};

/*************************** End of file ****************************/
