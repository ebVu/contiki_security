/*
 * Copyright (c) 2012, Texas Instruments Incorporated - http://www.ti.com/
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/** \addtogroup cc2538-examples
 * @{
 *
 * \defgroup cc2538-echo-server cc2538dk UDP Echo Server Project
 *
 *  Tests that a node can correctly join an RPL network and also tests UDP
 *  functionality
 * @{
 *
 * \file
 *  An example of a simple UDP echo server for the cc2538dk platform
 */
#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>

#define DEBUG DEBUG_PRINT
#include "net/uip-debug.h"
#include "dev/watchdog.h"
#include "dev/leds.h"
#include "net/rpl/rpl.h"
// #include "TI_aes.h"
#include "aes.h"
/*---------------------------------------------------------------------------*/
#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[uip_l2_l3_hdr_len])

#define MAX_PAYLOAD_LEN 120
#define ECB 1
/*---------------------------------------------------------------------------*/
static struct uip_udp_conn *server_conn;
static char buf[MAX_PAYLOAD_LEN];
static uint16_t len;
static uint8_t data_buffer[16];
static void print_array(char* pstring, int length);
// static void prepare_descrypt();
static void test_encrypt_ecb();
static int uip_newdata_flag;
/*---------------------------------------------------------------------------*/
PROCESS(udp_echo_server_process, "UDP echo server process");
AUTOSTART_PROCESSES(&udp_echo_server_process);
/*---------------------------------------------------------------------------*/
void print_array(char* pstring, int length) {
    int i;
    for(i = 0; i < length; i += 8)
    {
      // printf("%x\n", (int*)(pstring + i));
     printf("{%0x, %0x, %0x, %0x, %0x, %0x, %0x}\n", *(uint8_t*)(pstring + i), *(uint8_t*)(pstring + i + 1), *(uint8_t*)(pstring + i + 2), *(uint8_t*)(pstring + i + 3), *(uint8_t*)(pstring + i + 4), *(uint8_t*)(pstring + i + 5), *(uint8_t*)(pstring + i + 6), *(uint8_t*)(pstring + i + 7));
    }
}
static void test_encrypt_ecb()
{
  uint8_t key[]   = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  printf("input data");
  print_array(buf, 16);                       
  AES128_ECB_encrypt(buf, key, data_buffer);
  printf("output data");
  print_array(data_buffer, 16); 
}

// void prepare_descrypt() 
// {
//   uint8_t key[]   = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
//                            0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
//   memcpy(&data_buffer, buf, 16);
//   print_array(&data_buffer, 16);
//   aes_decrypt(data_buffer,key);
//   print_array(&data_buffer, 16);

// }

// static void
// tcpip_handler(void)
// {
//   memset(buf, 0, MAX_PAYLOAD_LEN);
//   if(uip_newdata()) {
//     leds_on(LEDS_RED);
//     len = uip_datalen();
//     memcpy(buf, uip_appdata, len);

//     PRINTF("%u bytes from [", len);
//     PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
//     PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
//     uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
//     server_conn->rport = UIP_UDP_BUF->srcport;

//     // prepare_descrypt();

//     uip_udp_packet_send(server_conn, buf, len);
//     uip_create_unspecified(&server_conn->ripaddr);
//     server_conn->rport = 0;
//   }
//   leds_off(LEDS_RED);
//   return;
// }
static void
tcpip_handler1(void)
{
  memset(buf, 0, MAX_PAYLOAD_LEN);
  if(uip_newdata()) {
    uip_newdata_flag = 1;
    leds_on(LEDS_RED);
    len = uip_datalen();
    memcpy(buf, uip_appdata, len);

    PRINTF("%u bytes from [", len);
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF("]:%u\n", UIP_HTONS(UIP_UDP_BUF->srcport));
    uip_ipaddr_copy(&server_conn->ripaddr, &UIP_IP_BUF->srcipaddr);
    server_conn->rport = UIP_UDP_BUF->srcport;
  }
  return;
}
static void
tcpip_handler2(void)
{
    if (uip_newdata_flag)
    {
        uip_udp_packet_send(server_conn, data_buffer, len);
        uip_create_unspecified(&server_conn->ripaddr);
        server_conn->rport = 0;
    }
    leds_off(LEDS_RED);
    return;
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(udp_echo_server_process, ev, data)
{

  PROCESS_BEGIN();
  PRINTF("Starting UDP echo server\n");
  uip_newdata_flag = 0;
  server_conn = udp_new(NULL, UIP_HTONS(0), NULL);
  udp_bind(server_conn, UIP_HTONS(3000));

  PRINTF("Listen port: 3000, TTL=%u\n", server_conn->ttl);

  while(1) {
    PROCESS_YIELD();
    if(ev == tcpip_event) {
      // tcpip_handler();
        tcpip_handler1();
        test_encrypt_ecb();
        tcpip_handler2();
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/**
 * @}
 * @}
 */
