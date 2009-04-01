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

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "led_matrix.h"

#define LMS_PORT 9328

#include "led_matrix.h"
#include "arial_bold_14.h"

#ifdef LED_HEADFIRST
#define PUT_PIXEL(buffer,x,y) *(((uint16_t*)buffer)+((16*4-1-(x))/16)*16+(15-(y))) |= 1 << ((16*4-1-(x))%16)
#else
#define PUT_PIXEL(buffer,x,y) *(((uint16_t*)buffer)+((x)/16)*16+(y)) |= 1 << ((x)%16)
#endif


static uint8_t *font = Arial_Bold_14;

static int client_sock;

int led_matrix_init(char *matrix_ip)
{
    struct sockaddr_in server,client;
    int recv_size;
    int send_size;
    unsigned char command;

    client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(client_sock < 0)
        printf("Client_sock konnte nicht erstellt werden\n");
    server.sin_family = AF_INET;
    server.sin_port = htons(LMS_PORT);
    inet_aton(matrix_ip, &server.sin_addr);


    if(connect(client_sock, (struct sockaddr*)&server, sizeof(server)) != 0)
    {
        printf("Konnte nicht verbinden\n");
        return -1;
    }

    return 0;
}

void led_matrix_finish()
{
    close(client_sock);
}

static uint8_t *get_char_start(char c)
{
    uint8_t first_char = font[4];
    uint8_t char_height = font[3];
    uint8_t char_count = font[5];

    uint8_t factor = 1;

    if(char_height > 8)
        factor = 2;

    uint8_t counter;
    uint16_t position = 0;

    for(counter=0;counter<(c-first_char);counter++)
        position += font[6+counter]*factor;

    return font+(6+char_count+position);
}


int16_t putChar(uint16_t *buffer, char c,int16_t offset_x, int16_t offset_y)
{
    uint8_t first_char = font[4];
    uint8_t char_count = font[5];
    uint8_t char_width;
    uint8_t char_height = font[3];

    uint8_t *char_start = get_char_start(c);

    int16_t x,y;

    /* if char is not in our font just leave */
    if(c < first_char || c > (first_char + char_count))
        return 0;

    /* Leerzeichen abfangen */
    if(c == ' ')
        return 4;

    char_width = font[6+c-first_char];

    /* box level clipping */
    //if((ledLine->x + char_width) >= LINE_LENGTH-50)
    //    return 0;

    for (x=0;x<char_width;x++) for (y=0;y<char_height;y++)
    if (*(char_start+x+(y/8)*char_width) & (1 << (y/8)*(8-char_height%8) << (y%8)))
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

int16_t led_matrix_print(char *msg, struct _ledLine *ledLine) {
    return putString(ledLine->buffer_red,ledLine->buffer_green,msg,ledLine->x,ledLine->y);
}

int led_matrix_allocate_line(struct _ledLine *ledLine)
{
    ledLine->buffer_red = calloc(sizeof(uint16_t), 4*16);
    if(!ledLine->buffer_red)
        return 0;
    ledLine->buffer_green = calloc(sizeof(uint16_t), 4*16);
    if(!ledLine->buffer_green)
        return 0;

    ledLine->x = 0;
    ledLine->y = 1;

    led_matrix_clear_screen(ledLine);

    return 1;
}

void led_matrix_clear_screen(struct _ledLine *ledLine)
{
    memset(ledLine->buffer_red,0,sizeof(uint16_t)*4*16);
    memset(ledLine->buffer_green,0,sizeof(uint16_t)*4*16);
}

void led_matrix_update(struct _ledLine *ledLine)
{
    int bytes_send;

    bytes_send = send(client_sock, ledLine->buffer_red, sizeof(uint16_t)*4*16,0);
    bytes_send = send(client_sock, ledLine->buffer_green, sizeof(uint16_t)*4*16,0);
}

