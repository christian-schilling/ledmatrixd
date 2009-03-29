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

static uint16_t charGetStart(char c);
static void clearScreen();

/* Diese Arrays werden nur zur Uebertragung ans Modul genutzt */
static uint16_t RED[4][16];
static uint16_t GREEN[4][16];

static uint8_t *font = Arial_Bold_14;

static int client_sock;

static struct _ledLine *ledLine = NULL;

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
    if(ledLine)
        free(ledLine);
    close(client_sock);
}

static uint16_t charGetStart(char c)
{
    uint8_t first_char = font[4];
    uint8_t char_height = font[3];

    uint8_t factor = 1;

    if(char_height > 8)
        factor = 2;

    uint8_t counter;
    uint16_t position = 0;

    for(counter=0;counter<(c-first_char);counter++)
    {
        position += font[6+counter]*factor;
    }

    return position;
}

int putChar(char c, uint8_t color)
{

    uint8_t first_char = font[4];
    uint8_t char_count = font[5];
    uint8_t char_width;
    uint8_t char_height = font[3];

    uint16_t start;

    uint8_t i;

    if(c == '\n')
    {
        ledLine->x = 0;
        ledLine->y = 8;
        return 1;
    }

    /* if char is not in our font just leave */
    if(c < first_char || c > (first_char + char_count))
        return 1;
    
    /* Leerzeichen abfangen */
    if(c == 32)
        char_width = 4;
    else
    {
        char_width = font[6+c-first_char];
        start = charGetStart(c);
    }
    
    if((ledLine->x + char_width) >= LINE_LENGTH-50)
        return 0;

    if(c == ' ')
    {
        ledLine->x += 4;
        return 1;
    }

    for(i=0;i<char_width;i++)
    {
        if(color == COLOR_RED)
        {
            ledLine->column_red[i+ledLine->x] |= font[6+char_count+start+i]<<ledLine->y;
        }
        else if(color == COLOR_GREEN)
        {
            ledLine->column_green[i+ledLine->x] |= font[6+char_count+start+i]<<ledLine->y;
        }
        else if(color == COLOR_AMBER)
        {
            ledLine->column_red[i+ledLine->x] |= font[6+char_count+start+i]<<ledLine->y;
            ledLine->column_green[i+ledLine->x] |= font[6+char_count+start+i]<<ledLine->y;
        }
    }
    /* unteren Teil der Zeichen schreiben (noch nicht dynamisch fuer verschiedene Schriftgroessen) */
    for(i=0;i<char_width;i++)
    {
        if(color == COLOR_RED)
        {
            /* Man erklaere mir was ich hier geschrieben. Aber funktionieren tuts! :-) */
            ledLine->column_red[i+ledLine->x] |= font[6+char_count+start+i+char_width]<<(char_height - 8)<<ledLine->y;
        }
        else if(color == COLOR_GREEN)
        {
            ledLine->column_green[i+ledLine->x] |= font[6+char_count+start+i+char_width]<<(char_height - 8)<<ledLine->y;
        }
        else if(color == COLOR_AMBER)
        {
            ledLine->column_red[i+ledLine->x] |= font[6+char_count+start+i+char_width]<<(char_height - 8)<<ledLine->y;
            ledLine->column_green[i+ledLine->x] |= font[6+char_count+start+i+char_width]<<(char_height -8)<<ledLine->y;
        }
    }

    ledLine->x += char_width + 1;

    return 1;
}

void putString(char *string)
{
    static int color = COLOR_RED;

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
        else if(!putChar(*string,color))
            return;
        string++;
    }
}

void led_matrix_print(char *msg) {
    if(!ledLine)
    {
        ledLine = (void*)malloc(sizeof(struct _ledLine));
        allocateLedLine(ledLine, LINE_LENGTH);
    }
    putString(msg);
}

int allocateLedLine(struct _ledLine *ledLine, int line_length)
{
	ledLine->column_red = calloc(sizeof(uint16_t), line_length);
	if(!ledLine->column_red)
		return 0;
	ledLine->column_green = calloc(sizeof(uint16_t), line_length);
	if(!ledLine->column_green)
		return 0;
	
	ledLine->column_red_output = calloc(sizeof(uint16_t), line_length);
	if(!ledLine->column_red_output)
		return 0;
	ledLine->column_green_output = calloc(sizeof(uint16_t), line_length);
	if(!ledLine->column_green_output)
		return 0;
	

	clearScreen(ledLine);

	ledLine->x = 0;
	ledLine->y = 1;
	ledLine->shift_position = 0;
	return 1;
}

static void clearScreen()
{
    memset(ledLine->column_red,0,sizeof(uint16_t)*LINE_LENGTH);
    memset(ledLine->column_green,0,sizeof(uint16_t)*LINE_LENGTH);
    
    memset(ledLine->column_red_output,0,sizeof(uint16_t)*LINE_LENGTH);
    memset(ledLine->column_green_output,0,sizeof(uint16_t)*LINE_LENGTH);

    ledLine->x = 0;
    ledLine->y = 1;
}


int shiftLeft()
{
    int counter;

    int scroll_length;
    
    if(ledLine->x + 11 > LINE_LENGTH)
        scroll_length = LINE_LENGTH;
    else
        scroll_length = ledLine->x + 11;

    for(counter=0;counter< scroll_length - 1  ;counter++)
    {
        if(ledLine->shift_position + counter > scroll_length - 1)
        {
            ledLine->column_red_output[counter] = ledLine->column_red[counter + ledLine->shift_position - (scroll_length)];
            ledLine->column_green_output[counter] = ledLine->column_green[counter + ledLine->shift_position - (scroll_length)];
        }
        else
        {
            ledLine->column_red_output[counter] = ledLine->column_red[ledLine->shift_position+counter];
            ledLine->column_green_output[counter] = ledLine->column_green[ledLine->shift_position+counter];
        }
    }

    ledLine->shift_position++;
    
    if(ledLine->shift_position > ledLine->x + 11)
    {
        ledLine->shift_position = 1;
        return 0;
    }
    else
        return 1;
}

void led_matrix_update()
{
    int bytes_send;
    int i,p,m;

    memset(&RED,0,sizeof(RED));
    memset(&GREEN,0,sizeof(GREEN));

    for(m=0;m<4;m++) // for every module
    {
        for(i=0;i<16;i++) // for every row
        {
            for(p=0;p<16;p++) // for every single led in row
            {
#ifdef LED_HEADFIRST
                /* was there a shift yet? if no, print the unshifted arrays */
                if(ledLine->shift_position)
                {
                    RED[3-m][15-i] |= ((ledLine->column_red_output[15-p+m*16] & (1<<i))>>(i)<<p);
                    GREEN[3-m][15-i] |= ((ledLine->column_green_output[15-p+m*16] & (1<<i))>>(i)<<p);
                }
                else
                {
                    RED[3-m][15-i] |= ((ledLine->column_red[15-p+m*16] & (1<<i))>>(i)<<p);
                    GREEN[3-m][15-i] |= ((ledLine->column_green[15-p+m*16] & (1<<i))>>(i)<<p);
                }
#else
                /* was there a shift yet? if no, print the unshifted arrays */
                if(ledLine->shift_position)
                {
                    RED[m][i] |= ((ledLine->column_red_output[p+m*16] & (1<<i))>>(i)<<p);
                    GREEN[m][i] |= ((ledLine->column_green_output[p+m*16] & (1<<i))>>(i)<<p);
                }
                else
                {
                    RED[m][i] |= ((ledLine->column_red[p+m*16] & (1<<i))>>(i)<<p);
                    GREEN[m][i] |= ((ledLine->column_green[p+m*16] & (1<<i))>>(i)<<p);
                }
#endif

            }
        }
    }
    bytes_send = send(client_sock, &RED, sizeof(RED),0);
    bytes_send = send(client_sock, &GREEN, sizeof(GREEN),0);

}

