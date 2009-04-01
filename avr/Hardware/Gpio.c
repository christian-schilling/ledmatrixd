/*
 * Code for controlling GPIOs. This is meant to be an extension
 * to the simple PORTx/PINx/DDRx Interface.
 * You simply can specify a various number of GPIOs that are
 * mapped to randomly specified pins on the avr. 
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

#include "Gpio.h"
#include <avr/pgmspace.h>
#include <avr/io.h>

struct tGpio
{
	volatile uint8_t* pPin;
	volatile uint8_t* pPort;
	volatile uint8_t* pDdr;
	uint8_t nBit;
};

struct tGpio PROGMEM g_Gpio[] =
{
	{	&PINC, &PORTC, &DDRC, PC0},
	{	&PINC, &PORTC, &DDRC, PC1},
	{	&PINC, &PORTC, &DDRC, PC2},
	{	&PINC, &PORTC, &DDRC, PC3},
	{	&PINC, &PORTC, &DDRC, PC4},
	{	&PINC, &PORTC, &DDRC, PC5},
	{	&PIND, &PORTD, &DDRD, PD0},
	{	&PIND, &PORTD, &DDRD, PD1},
	{	&PIND, &PORTD, &DDRD, PD2},
	{	&PIND, &PORTD, &DDRD, PD3},
	{	&PIND, &PORTD, &DDRD, PD4},
	{	&PIND, &PORTD, &DDRD, PD5},
	{	&PIND, &PORTD, &DDRD, PD6},
	{	&PIND, &PORTD, &DDRD, PD7}
};

uint8_t GpioGetPin(uint8_t nGpio)
{
	volatile uint8_t* pPin =
			(volatile uint8_t*) pgm_read_word(&(g_Gpio[nGpio].pPin));
	uint8_t nBit = pgm_read_byte(&(g_Gpio[nGpio].nBit));

	return (*pPin >> nBit) & 1;
}

uint8_t GpioGetPort(uint8_t nGpio)
{
	volatile uint8_t* pPort =
			(volatile uint8_t*) pgm_read_word(&(g_Gpio[nGpio].pPort));
	uint8_t nBit = pgm_read_byte(&(g_Gpio[nGpio].nBit));

	return (*pPort >> nBit) & 1;
}

uint8_t GpioGetDdr(uint8_t nGpio)
{
	volatile uint8_t* pDdr =
			(volatile uint8_t*) pgm_read_word(&(g_Gpio[nGpio].pDdr));
	uint8_t nBit = pgm_read_byte(&(g_Gpio[nGpio].nBit));

	return (*pDdr >> nBit) & 1;
}

void GpioSetPort(	uint8_t nGpio,
					uint8_t nState)
{
	volatile uint8_t* pPort =
			(volatile uint8_t*) pgm_read_word(&(g_Gpio[nGpio].pPort));
	uint8_t nBit = pgm_read_byte(&(g_Gpio[nGpio].nBit));

	if (nState)
		*pPort |= (1<<nBit);
	else
		*pPort &= ~(1<<nBit);
}

void GpioSetDdr(uint8_t nGpio,
				uint8_t nState)
{
	volatile uint8_t* pDdr =
			(volatile uint8_t*) pgm_read_word(&(g_Gpio[nGpio].pDdr));
	uint8_t nBit = pgm_read_byte(&(g_Gpio[nGpio].nBit));

	if (nState)
		*pDdr |= (1<<nBit);
	else
		*pDdr &= ~(1<<nBit);
}
