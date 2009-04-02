/*
 * Main File including the main-routine with initializations-
 * and stack-routines.
 *
 * Author: Simon Kueppers
 * Email: simon.kueppers@web.de
 * Homepage: http://klinkerstein.m-faq.de
 *
 *
 * Additions for the connected LED-Matrix-Display
 * made by Bjoern Biesenbach
 * */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "Hardware/Enc28j60.h"
#include "Hardware/Spi.h"
#include "Net/uip/uip.h"
#include "Net/uip/uip_arp.h"
#include "Net/uip/uip_TcpAppHub.h"
#include "led_matrix.h"

#define IP_ADDRESS1 192
#define IP_ADDRESS2 168
#define IP_ADDRESS3 0
#define IP_ADDRESS4 93

#define NETMASK1 255
#define NETMASK2 255
#define NETMASK3 0
#define NETMASK4 0

#define ROUTER1 192
#define ROUTER2 168
#define ROUTER3 0
#define ROUTER4 2

static uint8_t g_nPrescaler = 100;
static volatile struct
{
    uint8_t fPeriodic :1;
} g_nFlags =
{ 0 };

int main()
{
    uip_ipaddr_t IpAddr;
    char ip_addr_string[16];
    SpiInit();
    Enc28j60Init();
    uip_arp_init();
    uip_init();
    uip_TcpAppHubInit();
    Enc28j60SetClockPrescaler(2);

    TCCR1B = (1<<WGM12)|(3<<CS10);
    OCR1A = 19531; //12500000 / 64 / 19531,25 = 10Hz (100ms)
    TIMSK = (1<<OCIE1A);

    uip_ipaddr(IpAddr, IP_ADDRESS1, IP_ADDRESS2, IP_ADDRESS3, IP_ADDRESS4);
    uip_sethostaddr(IpAddr);
    uip_ipaddr(IpAddr, ROUTER1, ROUTER2, ROUTER3, ROUTER4);
    uip_setdraddr(IpAddr);
    uip_ipaddr(IpAddr, NETMASK1, NETMASK2, NETMASK3, NETMASK4);
    uip_setnetmask(IpAddr);

    // sprintf will increment flash usage. too lazy to write a replacement now
    sprintf(ip_addr_string,"%01d.%01d.%01d.%01d", IP_ADDRESS1,
        IP_ADDRESS2,
        IP_ADDRESS3,
        IP_ADDRESS4);
    led_init();

    sei ();
    
    putString(backbuffer,backbuffer+16*4,ip_addr_string,0,1);
    swap_buffers();

    while (1)
    {
        uip_len = Enc28j60Receive(uip_buf);

        if (uip_len> 0)
        {
            if (((struct uip_eth_hdr *)&uip_buf[0])->type
                    == htons(UIP_ETHTYPE_IP))
            {
                //uip_arp_ipin();
                uip_input();

                if (uip_len> 0)
                {
                    uip_arp_out();
                    Enc28j60CopyPacket(uip_buf, uip_len);
                    Enc28j60Send();
                }

            }
            else if (((struct uip_eth_hdr *)&uip_buf[0])->type
                    == htons(UIP_ETHTYPE_ARP))
            {
                uip_arp_arpin();
                if (uip_len> 0)
                {
                    Enc28j60CopyPacket(uip_buf, uip_len);
                    Enc28j60Send();
                }
            }
        }

        if (g_nFlags.fPeriodic)
        {
            cli();
            g_nFlags.fPeriodic = 0;
            sei();

            int i= UIP_CONNS;
            while (i)
            {
                i--;
                uip_periodic(i);
                if (uip_len> 0)
                {
                    uip_arp_out();
                    Enc28j60CopyPacket(uip_buf, uip_len);
                    Enc28j60Send();
                }
            }

            g_nPrescaler--;

            if (g_nPrescaler == 0)
            {
                //Every 10 seconds
                uip_arp_timer();
                g_nPrescaler = 100;
            }

        }
        /* Comment this out and your led-matrix will go up in flames */

        led_runner();
    }

    return 0;
}

ISR(TIMER1_COMPA_vect)
{
    g_nFlags.fPeriodic = 1;
}
