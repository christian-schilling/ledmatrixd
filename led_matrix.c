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

static void hilfsarray_to_normal(void);
static uint16_t charGetStart(char c);

/* Diese Arrays werden nur zur Uebertragung ans Modul genutzt */
static uint16_t RED[4][16];
static uint16_t GREEN[4][16];
static uint16_t tmpRED[4][16];
static uint16_t tmpGREEN[4][16];

/* Dies sind die eigentlich genutzten Arrays. Grund: 
 * einfachere Handhabung! Jedes Element entspricht genau einer Spalte
 */
static uint16_t column_red[512];
static uint16_t column_green[512];

static int position = 0;

static uint16_t x,y=1;

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

/* Achtung, funktioniert derzeit nur fuer Arial_Bold_14 ! */
static uint16_t charGetStart(char c)
{
    uint8_t first_char = Arial_Bold_14[4];
    uint8_t char_count = Arial_Bold_14[5];
    uint8_t char_width = Arial_Bold_14[6+c-first_char];
    uint8_t char_height = Arial_Bold_14[3];

    uint8_t factor = 1;

    if(char_height > 8)
        factor = 2;

    uint8_t counter;
    uint16_t position = 0;

    for(counter=0;counter<(c-first_char);counter++)
    {
        position += Arial_Bold_14[6+counter]*factor;
    }

    return position;
}


static void hilfsarray_to_normal(void)
{
    int i,p,m;
    memset(&RED,0,sizeof(RED));
    memset(&GREEN,0,sizeof(GREEN));

    for(m=0;m<4;m++)
    {
        for(i=0;i<16;i++)
        {
            for(p=0;p<16;p++)
            {
                RED[m][i+y] |= ((column_red[p+m*16] & (1<<i))>>(i)<<p);
                GREEN[m][i+y] |= ((column_green[p+m*16] & (1<<i))>>(i)<<p);
            }
        }
    }
}

void putChar(char c, uint8_t color)
{
    /* Leerzeichen abfangen */
    if(c == 32)
    {
        x += 4;
        return;
    }

    uint8_t first_char = Arial_Bold_14[4];
    uint8_t char_count = Arial_Bold_14[5];
    uint8_t char_width = Arial_Bold_14[6+c-first_char];

    uint16_t start = charGetStart(c);

    uint8_t i,p;

    for(i=0;i<char_width;i++)
    {
        if(color == COLOR_RED)
        {
            column_red[i+x] = Arial_Bold_14[6+char_count+start+i];
        }
        else if(color == COLOR_GREEN)
        {
            column_green[i+x] = Arial_Bold_14[6+char_count+start+i];
        }
        else if(color == COLOR_AMBER)
        {
            column_red[i+x] = Arial_Bold_14[6+char_count+start+i];
            column_green[i+x] = Arial_Bold_14[6+char_count+start+i];
        }
    }
    for(i=0;i<char_width;i++)
    {
        if(color == COLOR_RED)
        {
            /* Man erklaere mir was ich hier geschrieben. Aber funktionieren tuts! :-) */
            column_red[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
        }
        else if(color == COLOR_GREEN)
        {
            column_green[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
        }
        else if(color == COLOR_AMBER)
        {
            column_red[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
            column_green[i+x] |= Arial_Bold_14[6+char_count+start+i+char_width]<<6;
        }
    }

    hilfsarray_to_normal();

    /* Bei Bedarf wieder an den Anfang gehen */
    if(x + char_width +1 < 511)
        x += char_width + 1;
    else
        x = 0;
}

void putString(char *string, uint8_t color)
{
    memset(&column_red,0,sizeof(column_red));
    memset(&column_green,0,sizeof(column_green));
    while(*string)
    {
        putChar(*string++,color);
    }
    x = 0;
}

void led_matrix_print(char *msg) {
    putString(msg,COLOR_AMBER);
}

void clearScreen(void)
{
    memset(&column_red,0,sizeof(column_red));
    memset(&column_green,0,sizeof(column_green));
}


void shiftLeft(void)
{
    uint16_t buffer_red,buffer_green;
    int counter;
    
    buffer_red = column_red[0];
    buffer_green = column_green[0];

    for(counter=0;counter<511;counter++)
    {
        column_red[counter] = column_red[counter+1];
        column_green[counter] = column_green[counter+1];
    }

    column_red[511] = buffer_red;
    column_green[511] = buffer_green;

    if(position)
        position -= 1;
    else
        position = 511;
    hilfsarray_to_normal();
}

void led_matrix_update()
{
    int bytes_send;
    int x,y,z;
    memset(tmpGREEN,0,sizeof(uint16_t)*4*16);
    memset(tmpRED,0,sizeof(uint16_t)*4*16);

    for (x=0;x<4;x++) for (y=0;y<16;y++) for (z=0;z<16;z++){
        tmpGREEN[x][y] |= ((GREEN[3-x][15-y] & (1 << z)) >> z) << (15-z);
        tmpRED[x][y] |= ((RED[3-x][15-y] & (1 << z)) >> z) << (15-z);
    }
    bytes_send = send(client_sock, tmpRED, sizeof(RED),0);
    bytes_send = send(client_sock, tmpGREEN, sizeof(GREEN),0);
}
