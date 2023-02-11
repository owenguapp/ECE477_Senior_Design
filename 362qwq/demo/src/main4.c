#include "commands.h"
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fifo.h"
#include "tty.h"
#include "lcd.h"
#include "ff.h"
#include "midi.h"
#include "step-array.h"
#include <stdint.h>
#include <math.h>
#include "background.h"
#include "frog.h"
#include "gator.h"
#include "gator_left.h"
#include "gameover.h"
#include "winwin.h"

void main(){
    LCD_DrawPicture(0,0, (const Picture *) &winwin);
}
