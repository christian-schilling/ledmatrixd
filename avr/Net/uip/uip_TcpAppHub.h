/*
 * This file is meant to be an extension to the uip Stack.
 * The idea is, to implement an "Application Hub" that routes 
 * incoming data to the right Daemons and Clients, that are 
 * implemented.
 * Each Application has its own Socket-Structure. And all Socket Structures are
 * overlayed in memory internally.
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

#ifndef UIP_TCPAPPHUB_H_
#define UIP_TCPAPPHUB_H_

#include <stdint.h>
#include "../TcpApps/lms.h"

void uip_TcpAppHubCall(void);
void uip_TcpAppHubInit(void);

#define UIP_APPCALL    uip_TcpAppHubCall


typedef union 
{
	struct tLMS LMSSocket;
	//struct tFtp FtpSocket;
	//...
	
} uip_tcp_appstate_t;



#endif /*UIP_TCPAPPHUB_H_*/
