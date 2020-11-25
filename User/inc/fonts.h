#ifndef __FONT_H
#define __FONT_H

#include "stm32f10x.h"

typedef struct _tFont
{
  const uint8_t *table;
  uint16_t width;
  uint16_t height;
} sFONT;

extern sFONT Font24x32;
extern sFONT Font16x24;
extern sFONT Font8x16;

#endif /* __FONT_H */
