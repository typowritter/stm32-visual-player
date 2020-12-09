#ifndef __PLAYER_H
#define __PLAYER_H

#include "stm32f10x.h"

// DAC_DHR8R1寄存器，8位、右对齐、通道1
#define DAC_DHR8R1_ADDR      (DAC_BASE+0x10)
// DAC_DHR12R1寄存器，12位、右对齐、通道1
#define DAC_DHR12R1_ADDR     (DAC_BASE+0x08)

#define SAMPLE_RATE          8000

typedef enum
{
  PLAYING,
  PAUSED
} PlayerStatu;

void player_init(void);
void player_start(void);
void player_pause(void);
void player_stop(void);
void player_resume(void);
void check_reload(void);
PlayerStatu get_statu(void);


#endif /* __PLAYER_H */

