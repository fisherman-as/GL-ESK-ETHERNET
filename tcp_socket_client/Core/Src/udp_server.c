/*
 * UDP server
 * was written by Aleksey Yabanzhi
 * with help of Petro Shevchenko
 * 07.04.2023
 */
#include "simple_http_server.h"
#include "main.h"
#include "lwip.h"
#include "sockets.h"
#include "cmsis_os.h"
#include <string.h>

#define PORTNUM 5678UL
#define VERSION "udp_srv_Aleksey_Yabanzhi_07042023\n"

#if (USE_UDP_SERVER_PRINTF == 1)
#include <stdio.h>
#define UDP_SERVER_PRINTF(...) do { printf("[udp_server.c: %s: %d]: ",__func__, __LINE__);printf(__VA_ARGS__); } while (0)
#else
#define UDP_SERVER_PRINTF(...)
#endif

static struct sockaddr_in serv_addr, client_addr;
static int socket_fd;
static uint16_t nport;


static int udpServerInit(void)
{
	socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_fd == -1) {
		UDP_SERVER_PRINTF("socket() error\n");
		return -1;
	}

	nport = PORTNUM;
	nport = htons((uint16_t)nport);

	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = nport;

	if(bind(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))==-1) {
		UDP_SERVER_PRINTF("bind() error\n");
		close(socket_fd);
		return -1;
	}

	UDP_SERVER_PRINTF("Server is ready\n");

	return 0;
}

void StartUdpServerTask(void const * argument)
{
	osDelay(5000);// wait 5 sec to init lwip stack
	if(udpServerInit() < 0) {
		UDP_SERVER_PRINTF("udpSocketServerInit() error\n");
		osThreadTerminate(NULL);
	}

	for(;;)
	{
	  bzero(&client_addr, sizeof(client_addr));

	  int nbytes;
	  const size_t buf_len=256;
	  char buffer[buf_len];
	  memset(buffer, 8, buf_len);
	  socklen_t addrlen = sizeof(client_addr);

	  while ( (nbytes = recvfrom(socket_fd, buffer, (size_t)sizeof(buffer),
			  0, (struct sockaddr *)&client_addr, (socklen_t*)&addrlen)) > 0 )
	  {
//---------------------------------------------------------------------------------------------------------------------------------------------------
//exit
		  if (strncmp(buffer, "exit", strlen("exit")) == 0)
		{
			sendto(socket_fd, "goodbye, my friend!", strlen("goodbye, my friend!"), 0, (const struct sockaddr*)&client_addr, addrlen);
			break;
		}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//sversion
		  if (strncmp(buffer, "sversion", strlen("sversion")) == 0)
		{
			sendto(socket_fd, VERSION, strlen(VERSION), 0, (const struct sockaddr*)&client_addr, addrlen);
			sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
			continue;
		}
//---------------------------------------------------------------------------------------------------------------------------------------------------
		{
			/*---------------------------------Here we break the buffer into two different words-------------------------*/
			char* Command[2];
			char* pvCommand = strtok(buffer, " "); //the first command (led<n>) has to finish by SPACE
			Command[0] = pvCommand;
			pvCommand = strtok(NULL, "\n"); //the second command (on, off) has to finish by ENTER
			Command[1] = pvCommand;
			/*-----Now we have the first part of command in Command[0] and the second part of command in Command[1]------*/

//led3
			if (strcmp(Command[0], "led3") == 0)
			{
				if (strcmp(Command[1], "on") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "off") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "toggle") == 0)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "status") == 0)
				{
					GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_12);
					if (pinState==GPIO_PIN_SET)
					{sendto(socket_fd, "led3 ON\nOK\n", strlen("led3 ON\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
					else if (pinState==GPIO_PIN_RESET)
					{sendto(socket_fd, "led3 OFF\nOK\n", strlen("led3 OFF\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
				}
				else
				{sendto(socket_fd, "ERROR\n", strlen("ERROR\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
			continue;
			}
//led4
			if (strcmp(Command[0], "led4") == 0)
			{
				if (strcmp(Command[1], "on") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "off") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "toggle") == 0)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "status") == 0)
				{
					GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_13);
					if (pinState==GPIO_PIN_SET)
					{sendto(socket_fd, "led4 ON\nOK\n", strlen("led34 ON\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
					else if (pinState==GPIO_PIN_RESET)
					{sendto(socket_fd, "led4 OFF\nOK\n", strlen("led4 OFF\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
				}
				else
				{sendto(socket_fd, "ERROR\n", strlen("ERROR\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
			continue;
			}
//led5
			if (strcmp(Command[0], "led5") == 0)
			{
				if (strcmp(Command[1], "on") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "off") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "toggle") == 0)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_14);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "status") == 0)
				{
					GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14);
					if (pinState==GPIO_PIN_SET)
					{sendto(socket_fd, "led5 ON\nOK\n", strlen("led5 ON\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
					else if (pinState==GPIO_PIN_RESET)
					{sendto(socket_fd, "led5 OFF\nOK\n", strlen("led5 OFF\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
				}
				else
				{sendto(socket_fd, "ERROR\n", strlen("ERROR\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
			continue;
			}
//led6
			if (strcmp(Command[0], "led6") == 0)
			{
				if (strcmp(Command[1], "on") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "off") == 0)
				{
					HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "toggle") == 0)
				{
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_15);
					sendto(socket_fd, "OK\n", strlen("OK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);
				}
				else if (strcmp(Command[1], "status") == 0)
				{
					GPIO_PinState pinState = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_15);
					if (pinState==GPIO_PIN_SET)
					{sendto(socket_fd, "led6 ON\nOK\n", strlen("led6 ON\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
					else if (pinState==GPIO_PIN_RESET)
					{sendto(socket_fd, "led6 OFF\nOK\n", strlen("led6 OFF\nOK\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
				}
				else
				{sendto(socket_fd, "ERROR\n", strlen("ERROR\n"), 0, (const struct sockaddr*)&client_addr, addrlen);}
			continue;
			}
		}
//---------------------------------------------------------------------------------------------------------------------------------------------------
//echo
		if (sendto(socket_fd, buffer, nbytes, 0, (const struct sockaddr*)&client_addr, addrlen) < 0)
		{
			UDP_SERVER_PRINTF("send() error\n");
		}

	  }
			close(socket_fd);
	}
}

