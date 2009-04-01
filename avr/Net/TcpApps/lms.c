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
 *
 */

/*
 * This is the most simple server to obtain data for the 64x16 display
 */

#include "lms.h"
#include "../uip/uip.h"
#include "../../led_matrix.h"

void LMSInit()
{
	uip_listen(HTONS(PORT_LMS));
}

void LMSCall(	uint8_t* pBuffer,
				uint16_t nBytes,
				struct tLMS* pSocket)
{
	uint16_t counter;

	/* wird nur direkt nach connect ausgefuehrt */
	if(uip_connected())
	{
		pSocket->byte_counter = 0;
	}
	else if(uip_newdata() || uip_acked())
	{
		for(counter=0;counter < nBytes; counter++)
		{
			*(((uint8_t*)backbuffer)+(pSocket->byte_counter++)) = *pBuffer++;
			if(pSocket->byte_counter == 256)
			{
				swap_buffers();
				pSocket->byte_counter = 0;
			}
		}
	}
}

