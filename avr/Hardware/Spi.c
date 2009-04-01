/*
 * Source file containing functions and pre-compiler definitions
 * Note that this code module is not optimized on speed!
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

#include <avr/io.h>
#include "Spi.h"

void SpiInit()
{
#if (SPI_PRESCALER == 2)
	SPCR = (0<<SPR0)|(0<<SPR1);
	SPSR = (1<<SPI2X);
#elif (SPI_PRESCALER == 4)
	SPCR = (0<<SPR0)|(0<<SPR1);
	SPSR = (0<<SPI2X);
#elif (SPI_PRESCALER == 8)
	SPCR = (1<<SPR0)|(0<<SPR1);
	SPSR = (1<<SPI2X);
#elif (SPI_PRESCALER == 16)
	SPCR = (1<<SPR0)|(0<<SPR1);
	SPSR = (0<<SPI2X);
#elif (SPI_PRESCALER == 32)
	SPCR = (0<<SPR0)|(1<<SPR1);
	SPSR = (1<<SPI2X);
#elif (SPI_PRESCALER == 64)
	SPCR = (0<<SPR0)|(1<<SPR1);
	SPSR = (0<<SPI2X);
#elif (SPI_PRESCALER == 128)
	SPCR = (1<<SPR0)|(1<<SPR1);
	SPSR = (0<<SPI2X);
#else
#error Invalid SPI Prescaler selected
#endif

#if (SPI_MODE == 0)
	SPCR |= (0<<CPOL)|(0<<CPHA);
#elif (SPI_MODE == 1)
	SPCR |= (0<<CPOL)|(1<<CPHA);
#elif (SPI_MODE == 2)
	SPCR |= (1<<CPOL)|(0<<CPHA);
#elif (SPI_MODE == 3)
	SPCR |= (1<<CPOL)|(1<<CPHA);
#else
#error Invalid SPI Mode selected
#endif

	SPCR |= (1<<SPE)|(1<<MSTR);
}

void SpiWriteByte(uint8_t nByte)
{
	SPDR = nByte;
	while (!(SPSR & (1<<SPIF)))
	{
		asm volatile ("nop");
	}
}

void SpiWriteChunk(	const uint8_t* pChunk,
					uint16_t nBytes)
{
	while (nBytes--)
	{
		SPDR = *pChunk++;
		while (!(SPSR & (1<<SPIF)))
		{
			asm volatile ("nop");
		}
	}
}

uint8_t SpiReadByte()
{
	SPDR = 0;
	while (!(SPSR & (1<<SPIF)))
	{
		asm volatile ("nop");
	}

	return SPDR;
}

void SpiReadChunk(	uint8_t* pChunk,
					uint16_t nBytes)
{
	while (nBytes--)
	{
		SPDR = 0;
		while (!(SPSR & (1<<SPIF)))
		{
			asm volatile ("nop");
		}

		*pChunk++ = SPDR;
	}

}
