/*
 * Copyright (C) 2008 Bjoern Biesenbach <bjoern@bjoern-b.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "led_matrix.h"

#define LED_SELECT_PORT PORTA
#define LED_SELECT_DDRD DDRA

#define LED_SELECT_1 PA0
#define LED_SELECT_2 PA1
#define LED_SELECT_3 PA2
#define LED_SELECT_4 PA3

#define LED_CONTROL_PORT PORTC
#define LED_CONTROL_DDRD DDRC

#define LED_RESET PC0
#define LED_BRIGHT PC1
#define LED_CLOCK PC2
#define LED_GREEN PC3
#define LED_RED PC4
#define LED_BRTWRT PC5
#define LED_BRTCLK PC6

/* Arrays für Rot und Gruen
 * Ein uint16_t pro Zeile pro Modul
 */
static uint16_t BUFFERS[2*2*4*16];

static uint16_t *frontbuffer;
uint16_t *backbuffer;

void swap_buffers(void)
{
    uint16_t *tmp;
    tmp = backbuffer;
    backbuffer = frontbuffer;
    frontbuffer = tmp;
}

void led_update(void)
{
    uint8_t counter,counter2,i;

    /* reset */
    LED_CONTROL_PORT |= (1<<LED_RESET);
    LED_CONTROL_PORT &= ~(1<<LED_RESET);

    uint16_t *buf_ptr = frontbuffer;
    for(i=0;i<4;i++)
    {
        LED_SELECT_PORT |= (1<<i);
        for(counter2 = 0;counter2 < 16;counter2++)
        {
    //      LED_CONTROL_PORT |= (1<<LED_BRIGHT);
    //      LED_CONTROL_PORT |=  (1<<LED_BRIGHT);
            for(counter = 0; counter < 16; counter++)
            {
                if((*buf_ptr>>counter)&1)
                    LED_CONTROL_PORT |= (1<<LED_RED);
                else
                    LED_CONTROL_PORT &= ~(1<<LED_RED);
                if((*(buf_ptr+16*4)>>counter)&1)
                    LED_CONTROL_PORT |= (1<<LED_GREEN);
                else
                    LED_CONTROL_PORT &= ~(1<<LED_GREEN);
                LED_CONTROL_PORT |= (1<<LED_CLOCK);
                LED_CONTROL_PORT &= ~(1<<LED_CLOCK);
            }
            buf_ptr++;
    //      LED_CONTROL_PORT &= ~(1<<LED_BRIGHT);
        }
        LED_SELECT_PORT &= ~(1<<i);
    }
}

void led_init(void)
{
    /* Array initialisieren */
    memset(BUFFERS,0,sizeof(BUFFERS));

    frontbuffer = BUFFERS;
    backbuffer = BUFFERS + (2 * 16 * 4);

    LED_SELECT_DDRD |= (1<<LED_SELECT_1) |
        (1<<LED_SELECT_2) |
        (1<<LED_SELECT_3) |
        (1<<LED_SELECT_4);

    LED_CONTROL_DDRD |= (1<<LED_RESET) |
        (1<<LED_BRIGHT) |
        (1<<LED_CLOCK) |
        (1<<LED_GREEN) |
        (1<<LED_RED) |
        (1<<LED_BRTWRT) |
        (1<<LED_BRTCLK);

    LED_SELECT_PORT = 0;
    LED_CONTROL_PORT = 0;
    LED_CONTROL_PORT |= 1<<LED_BRTWRT;
}

void led_runner(void)
{
    led_update();

    LED_CONTROL_PORT |= (1<<LED_CLOCK);
    LED_CONTROL_PORT &= ~(1<<LED_CLOCK);
}

///////////////////////////////////////////////7
/////////////////////////////////////////////////

#define LED_HEADFIRST
#ifdef LED_HEADFIRST
#define PUT_PIXEL(buffer,x,y) *(((uint16_t*)buffer)+((16*4-1-(x))/16)*16+(15-(y))) |= 1 << ((16*4-1-(x))%16)
#else
#define PUT_PIXEL(buffer,x,y) *(((uint16_t*)buffer)+((x)/16)*16+(y)) |= 1 << ((x)%16)
#endif

#include "../arial_bold_14.h"
#include "../Comic_10.h"
PGM_P font_table[] = { Arial_Bold_14, Comic_10 };
PGM_P font = Comic_10;
#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_AMBER 2

PGM_P get_char_start(char c)
{
    uint8_t first_char = pgm_read_byte(font+4);
    uint8_t char_height = pgm_read_byte(font+3);
    uint8_t char_count = pgm_read_byte(font+5);

    uint8_t factor = 1;

    if(char_height > 8)
        factor = 2;

    uint8_t counter;
    uint16_t position = 0;

    for(counter=0;counter<(c-first_char);counter++)
        position += pgm_read_byte(font+6+counter)*factor;

    return font+(6+char_count+position);
}


int16_t putChar(uint16_t *buffer, char c,int16_t offset_x, int16_t offset_y)
{
    uint8_t first_char = pgm_read_byte(font+4);
    uint8_t char_count = pgm_read_byte(font+5);
    uint8_t char_width;
    uint8_t char_height = pgm_read_byte(font+3);

    PGM_P char_start = get_char_start(c);

    int16_t x,y;

    /* if char is not in our font just leave */
    if(c < first_char || c > (first_char + char_count))
        return 0;

    /* Leerzeichen abfangen */
    if(c == ' ')
        return 4;

    char_width = pgm_read_byte(font+6+c-first_char);

    /* box level clipping */
    //if((ledLine->x + char_width) >= LINE_LENGTH-50)
    //    return 0;

    for (x=0;x<char_width;x++) for (y=0;y<char_height;y++)
    if (pgm_read_byte(char_start+x+(y/8)*char_width) & (1 << (y/8)*(8-char_height%8) << (y%8)))
    if (0 <= x+offset_x && x+offset_x < 4*16) /* pixel level clipping */
    if (0 <= y+offset_y && y+offset_y < 16)   /* pixel level clipping */
        PUT_PIXEL(buffer,x+offset_x,y+offset_y);

    return char_width + 1;
}

int16_t putString(uint16_t *buffer_red,uint16_t *buffer_green, char *string, int16_t x, int16_t y)
{
    static int color = COLOR_RED;
    int16_t width = 0;

    if(!string)
        string = "null";
    while(*string)
    {
        if(*string == '\b')
            color = COLOR_GREEN;
        else if(*string == '\r')
            color = COLOR_RED;
        else if(*string == '\a')
            color = COLOR_AMBER;
        else if(color == COLOR_RED)
            width += putChar(buffer_red,*string,x+width,y);
        else if(color == COLOR_GREEN)
            width += putChar(buffer_green,*string,x+width,y);
        else if(color == COLOR_AMBER){
            putChar(buffer_green,*string,x+width,y);
            width += putChar(buffer_red,*string,x+width,y);
        }
        string++;
    }

    return width;
}

