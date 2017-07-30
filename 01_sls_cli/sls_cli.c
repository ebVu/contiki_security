/*
Shell cmd for controlling the SLS
author Vo Que Son <sonvq@hcmut.edu.vn>
*/
 
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
// #include "TI_aes.h"
#include "sls.h"
#define security_en (1)
#ifdef security_en
#endif
#include "aes.h"
#define KEY_LENGTH (16)
#define MAXBUF 100
// #define MAX_LENGTH 1024
// #define DELIMS " \t\r\n"

// static    int s_sock;
static  int     rev_bytes;
static  struct  sockaddr_in6 rev_sin6;
static  int     rev_sin6len;
static  char    rev_buffer[MAXBUF];
static  int     port;
static  char    dst_ipv6addr[50];
static  char    str_port[5];
static  char    cmd[20];
static  char    arg[20];
static  cmd_struct_t  tx_cmd, rx_reply;
static  cmd_struct_t *cmdPtr;
static  char *p;
static uint8_t data_buffer[16];
/*prototype definition */
static void print_cmd();
static void prepare_cmd();
static void prepare_encrypt();
static void print_array(char* pstring, int length);
struct timeval timer_usec; 
long long int timestamp_usec;
/*------------------------------------------------*/
long long int display_time()
{
  if (!gettimeofday(&timer_usec, NULL)) {
    timestamp_usec = ((long long int) timer_usec.tv_sec) * 1000000ll + 
                        (long long int) timer_usec.tv_usec;
  }
  else {
    timestamp_usec = -1;
  }
  return timestamp_usec;
}
void print_array(char* pstring, int length) {
int i;
for(i = 0; i < length; i += 8)
{
  // printf("%x\n", (int*)(pstring + i));
 printf("{%0x, %0x, %0x, %0x, %0x, %0x, %0x}\n", *(uint8_t*)(pstring + i), *(uint8_t*)(pstring + i + 1), *(uint8_t*)(pstring + i + 2), *(uint8_t*)(pstring + i + 3), *(uint8_t*)(pstring + i + 4), *(uint8_t*)(pstring + i + 5), *(uint8_t*)(pstring + i + 6), *(uint8_t*)(pstring + i + 7));
}
}
void prepare_encrypt() {
  uint8_t key[]   = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                           0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
  //memcpy(&data_buffer, &tx_cmd, 16);
  print_array(&data_buffer, 16);
  /* when using TI_aes.h */
  // aes_encrypt(data_buffer,key);

  /* when using tiny aes 128 */
  AES128_ECB_encrypt(&tx_cmd, key, data_buffer);
  print_array(&data_buffer, 16);

}
/*------------------------------------------------*/
void prepare_cmd() {
  tx_cmd.sfd = 0x7E;
  tx_cmd.len = sizeof(tx_cmd);
  tx_cmd.seq ++;
  tx_cmd.type = MSG_TYPE_REQ;
  tx_cmd.err_code = 0;  
}


/*------------------------------------------------*/
void print_cmd(cmd_struct_t command) {
  printf("SFD=0x%X; ",command.sfd);
  printf("len=%d; ",command.len);
  printf("seq=%d; ",command.seq);
  printf("type=0x%X; ",command.type);
  printf("cmd=0x%X; ",command.cmd);
  printf("err_code=0x%X; ",command.err_code);  
  printf("arg0=0x%X; ",command.arg[0]);
  printf("arg1=%d; ",command.arg[1]);
  printf("arg2=%d; ",command.arg[2]);
  printf("arg3=%d; ",command.arg[3]);
  printf("arg4=0x%X;\n",command.arg[4]);
}



int main(int argc, char* argv[])
{
  int sock;
  int status;
  struct addrinfo sainfo, *psinfo;
  struct sockaddr_in6 sin6;
  int sin6len;
  char buffer[MAXBUF];
  int i;

  sin6len = sizeof(struct sockaddr_in6);

  sprintf(buffer,"led_off");
	port = 3000;
  sprintf(dst_ipv6addr,"aaaa::212:7401:1:101");
	
  if(argc < 4) {
    printf("Specify an IPv6 addr or port number or Cmd \n"), exit(1);
	}
	else if (argc==4) {
    sprintf(dst_ipv6addr,argv[1]);      
    strcpy(str_port,argv[2]);
    strcpy(cmd,argv[3]);  
    port = atoi(str_port);
    sprintf(buffer,cmd);
    if (strcmp(cmd,"SLS_LED_ON")==0)
      tx_cmd.cmd = CMD_LED_ON;
    else if (strcmp(cmd,"CMD_SLS_LED_OFF")==0)
      tx_cmd.cmd = CMD_LED_OFF;    
    else if (strcmp(cmd,"SLS_GET_LED_STATUS")==0)
      tx_cmd.cmd = CMD_GET_LED_STATUS;
    else if (strcmp(cmd,"SLS_GET_NW_STATUS")==0)
      tx_cmd.cmd = CMD_GET_NW_STATUS;
    else if (strcmp(cmd,"SLS_GET_GW_STATUS")==0)
      tx_cmd.cmd = CMD_GET_GW_STATUS;
	}		
	else if (argc==5) {
    sprintf(dst_ipv6addr,argv[1]);      
    strcpy(str_port,argv[2]);
    sprintf(cmd,argv[3]);
    sprintf(arg,argv[4]);
		//sprintf(buffer,argv[2]);

    if (strcmp(cmd,"SLS_LED_DIM")==0) {
      tx_cmd.cmd = CMD_LED_DIM;    
      tx_cmd.arg[0] = atoi(arg);
    }

    port = atoi(str_port);
   	sprintf(buffer,"%s %s",cmd,arg);
	}	
  prepare_cmd();
  prepare_encrypt();


  strtok(buffer, "\n");

  sock = socket(PF_INET6, SOCK_DGRAM,0);

  memset(&sin6, 0, sizeof(struct sockaddr_in6));
  sin6.sin6_port = htons(port);
  sin6.sin6_family = AF_INET6;
  sin6.sin6_addr = in6addr_any;

  status = bind(sock, (struct sockaddr *)&sin6, sin6len);

  if(-1 == status)
    perror("bind"), exit(1);

  memset(&sainfo, 0, sizeof(struct addrinfo));
  memset(&sin6, 0, sin6len);

  sainfo.ai_flags = 0;
  sainfo.ai_family = PF_INET6;
  sainfo.ai_socktype = SOCK_DGRAM;
  sainfo.ai_protocol = IPPROTO_UDP;
  status = getaddrinfo(dst_ipv6addr, str_port, &sainfo, &psinfo);

  //status = sendto(sock, buffer, strlen(buffer), 0,
  //                   (struct sockaddr *)psinfo->ai_addr, sin6len);

   status = sendto(sock, &data_buffer, 16, 0,
                      (struct sockaddr *)psinfo->ai_addr, sin6len);
   // status = sendto(sock, &tx_cmd, sizeof(tx_cmd), 0,
   //                    (struct sockaddr *)psinfo->ai_addr, sin6len);
  timestamp_usec = display_time();
  printf("time when sending command is %lld\n", timestamp_usec);
  printf("Send REQUEST (len=%d) to [%s]:%s\n",status, dst_ipv6addr,str_port);
  print_cmd(tx_cmd);

  /*wait for a reply */
	rev_bytes = recvfrom(sock, rev_buffer, MAXBUF, 0,(struct sockaddr *)&rev_sin6, &rev_sin6len);
	if (rev_bytes<0) {
    perror("Problem in recvfrom \n");
    exit(1);
  }
  else {
    timestamp_usec = display_time();
    printf("time when get respone is %lld\n", timestamp_usec);
    printf("Got REPLY (len=%d):\n",rev_bytes);   
    p = (char *) (&rev_buffer); 
    cmdPtr = (cmd_struct_t *)p;
    rx_reply = *cmdPtr;
    print_cmd(rx_reply);      
  }



  shutdown(sock, 2);
  close(sock); 

   // free memory
  freeaddrinfo(psinfo);
  psinfo = NULL;
  return 0;
}

