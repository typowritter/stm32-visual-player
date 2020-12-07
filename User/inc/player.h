#ifndef __PLAYER_H
#define __PLAYER_H

#include "stm32f10x.h"
#include "bsp_sdio_sdcard.h"
#include "ff.h"

// DAC_DHR8R1寄存器，8位、右对齐、通道1
#define DAC_DHR8R1_ADDR      (DAC_BASE+0x10)


void player_init(void);
void player_start(void);
void mediainfo(uint8_t* pStr);


#endif /* __PLAYER_H */

