/*
 * Defines and function declarations for the Udp Application Hub
 * See uip_TcpAppHub for Explanations to this.
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

#ifndef UIP_UDPAPPHUB_H_
#define UIP_UDPAPPHUB_H_

#include <stdint.h>

void uip_UdpAppHubCall(void);
void uip_UdpAppHubInit(void);

#define UIP_UDP_APPCALL    uip_UdpAppHubCall

typedef struct
{
	uint8_t State;
} uip_udp_appstate_t;

#endif /*UIP_UDPAPPHUB_H_*/
