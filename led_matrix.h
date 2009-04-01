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

#ifndef __LED_ROUTINES_H__
#define __LED_ROUTINES_H__

#include <stdint.h>

#define COLOR_RED 0
#define COLOR_GREEN 1
#define COLOR_AMBER 2

#define LINE_LENGTH 512
//#define LED_HEADFIRST

/** Struct holding a line drawable at the display
 *
 */
struct _ledLine
{
    uint16_t *buffer_green;
    uint16_t *buffer_red;
    int8_t x; /**< current x position */
    int8_t y; /**< current y position */
};

typedef struct _ledLine led_matrix_line;

extern int led_matrix_init(char *matrix_ip);
extern void led_matrix_finish();
extern void led_matrix_update(struct _ledLine *ledLine);
extern void led_matrix_print(char *msg, struct _ledLine *ledLine);
extern int led_matrix_shift_left(struct _ledLine *ledLine);
extern int led_matrix_allocate_line(struct _ledLine *ledLine);
extern void led_matrix_clear_screen(struct _ledLine *ledLine);

#endif

