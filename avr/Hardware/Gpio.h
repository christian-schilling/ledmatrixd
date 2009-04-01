/*
 * Function prototypes for using with the GPIO code module
 * 
 * Author: Simon Kueppers
 * Email: simon.kueppers@web.de
 * Homepage: http://klinkerstein.m-faq.de
 * 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 Copyright 2008 Simon Kueppers
 * */

#ifndef GPIO_H_
#define GPIO_H_

#include <stdint.h>

uint8_t GpioGetPin(uint8_t nGpio);
uint8_t GpioGetPort(uint8_t nGpio);
void GpioSetPort(uint8_t nGpio, uint8_t nState);
uint8_t GpioGetDdr(uint8_t nGpio);
void GpioSetDdr(uint8_t nGpio, uint8_t nState);

#endif /*GPIO_H_*/
