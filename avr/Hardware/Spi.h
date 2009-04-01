/*
 * Defines and function-declarations regarding the Spi Code Module
 * which is used by the ENC28J60 Code Module
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

#ifndef SPI_H_
#define SPI_H_

#include <stdint.h>

#define SPI_PRESCALER 	2
#define SPI_MODE		0

void SpiInit();
void SpiWriteByte(uint8_t nByte);
void SpiWriteChunk(	const uint8_t* pChunk,
					uint16_t nBytes);
uint8_t SpiReadByte();
void SpiReadChunk(	uint8_t* pChunk,
					uint16_t nBytes);

#endif /*SPI_H_*/
