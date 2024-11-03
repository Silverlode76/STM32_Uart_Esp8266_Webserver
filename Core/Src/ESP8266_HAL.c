/*
 * ESP8266_HAL.c
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllerstech
 */


#include "UartRingbuffer_multi.h"
#include "ESP8266_HAL.h"
#include "stdio.h"
#include "string.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

Led_state Led1;
Led_state Led2;


#define wifi_uart &huart1
#define pc_uart &huart3


char buffer[20];


char *Basic_inclusion = "<!DOCTYPE html><html><meta name=\"viewport\"\
		content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n \
	<title>LED CONTROL</title>\n \
<style>html { font-family: Helvetica; 		display: inline-block; margin: 0px auto; text-align: center;} \
		body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}		h3 {color: #444444;margin-bottom: 50px;} \
.button {display: block;\
width: 80px;background-color: #1abc9c;border: none;color: white;\		
padding: 13px 30px;text-decoration: none;font-size: 25px;	\
margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\
.button1-on {background-color: #1abc9c;\
.button2-on {background-color: #1abc9c;}\n \
.button1-on:active 		{background-color: #16a085;} \n \
.button1-off {background-color: #34495e;} \n \
.button1-off:active {background-color: #2c3e50;} \n \
.button2-on:active 		{background-color: #16a085;} \n \
.button2-off {background-color: #34495e;} \n \
.button2-off:active {background-color: #2c3e50;} \n \
p {font-size: 14px;color: #888;margin-bottom: 10px;} \n \
</style> \n \
</head> \n \
<body>\n \
<h1>ESP8266 LED CONTROL</h1>\n";

char *LED1_ON = "<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>";
char *LED1_OFF = "<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>";
char *LED2_ON = "<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>";
char *LED2_OFF = "<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>";
char *Terminate = "</body></html>";



/*****************************************************************************************************************************************/

void ESP_Init (char *SSID, char *PASSWD)
{
	char data[80];

	Ringbuf_init();

	Uart_sendstring("AT+RST\r\n", wifi_uart);
	Uart_sendstring("RESETTING.", pc_uart);
	for (int i=0; i<5; i++)
	{
		Uart_sendstring(".", pc_uart);
		HAL_Delay(1000);
	}

	/********* AT **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT\r\n", wifi_uart);
	while(!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("AT---->OK\n\n", pc_uart);


	/********* AT+CWMODE=1 **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CWMODE=1\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("CW MODE---->1\n\n", pc_uart);


	/********* AT+CWJAP="SSID","PASSWD" **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("connecting... to the provided AP\n", pc_uart);
	sprintf (data, "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASSWD);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	sprintf (data, "Connected to,\"%s\"\n\n", SSID);
	Uart_sendstring(data,pc_uart);


	/********* AT+CIFSR **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIFSR\r\n", wifi_uart);
	while (!(Wait_for("CIFSR:STAIP,\"", wifi_uart)));
	while (!(Copy_upto("\"",buffer, wifi_uart)));
	while (!(Wait_for("OK\r\n", wifi_uart)));
	int len = strlen (buffer);
	buffer[len-1] = '\0';
	sprintf (data, "IP ADDR: %s\n\n", buffer);
	Uart_sendstring(data, pc_uart);

	/********* AT+CIPMUX **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIPMUX=1\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("CIPMUX---->OK\n\n", pc_uart);

	/********* AT+CIPSERVER **********/
	Uart_flush(wifi_uart);
	Uart_sendstring("AT+CIPSERVER=1,80\r\n", wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring("CIPSERVER---->OK\n\n", pc_uart);

	Uart_sendstring("Now Connect to the IP ADRESS\n\n", pc_uart);

}




int Server_Send (char *str, int Link_ID)
{
	int len = strlen (str);
	char data[80];
	sprintf (data, "AT+CIPSEND=%d,%d\r\n", Link_ID, len);
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for(">", wifi_uart)));
	Uart_sendstring(data, pc_uart);
	Uart_sendstring("---->OK\n\n", pc_uart);
	Uart_sendstring (str, wifi_uart);
	Uart_sendstring(str, pc_uart);
	while (!(Wait_for("SEND OK", wifi_uart)));
	Uart_sendstring("SEND---->OK\n\n", pc_uart);
	sprintf (data, "AT+CIPCLOSE=5\r\n");
	Uart_sendstring(data, wifi_uart);
	while (!(Wait_for("OK\r\n", wifi_uart)));
	Uart_sendstring(data, pc_uart);
	Uart_sendstring("--->OK\n\n", pc_uart);
	return 1;
}

void Server_Handle (char *str, int Link_ID)
{
	char datatosend[2048] = {0};
	if (!(strcmp (str, "/led1on")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED1_ON);
		if(Led2.off == 1) strcat(datatosend, LED2_OFF);
		else strcat(datatosend, LED2_ON);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/led1off")))
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED1_OFF);
		if(Led2.off == 1) strcat(datatosend, LED2_OFF);
		else strcat(datatosend, LED2_ON);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}
	else if (!(strcmp (str, "/led2on")))
	{
		sprintf (datatosend, Basic_inclusion);
		if(Led1.off == 1) strcat(datatosend, LED1_OFF);
		else strcat(datatosend, LED1_ON);
		strcat(datatosend, LED2_ON);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

	else if (!(strcmp (str, "/led2off")))
	{
		sprintf (datatosend, Basic_inclusion);
		if(Led1.off == 1) strcat(datatosend, LED1_OFF);
		else strcat(datatosend, LED1_ON);
		strcat(datatosend, LED2_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}
	else
	{
		sprintf (datatosend, Basic_inclusion);
		strcat(datatosend, LED1_OFF);
		strcat(datatosend, LED2_OFF);
		strcat(datatosend, Terminate);
		Server_Send(datatosend, Link_ID);
	}

}

void Server_Start (void)
{
	char buftocopyinto[128] = {0};
	char Link_ID;
	while (!(Get_after("+IPD,", 1, &Link_ID, wifi_uart)));

	Link_ID -= 48;
	while (!(Copy_upto(" HTTP/1.1", buftocopyinto, wifi_uart)));
	Uart_sendstring(buftocopyinto, pc_uart);
	if (Look_for("/led1on", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		Led1.off = 0x0;
		Server_Handle("/led1on",Link_ID);
	}

	else if (Look_for("/led1off", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		Led1.off = 0x1;
		Server_Handle("/led1off",Link_ID);
	}
	else if (Look_for("/led2on", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
		Led2.off = 0x0;
		Server_Handle("/led2on",Link_ID);
	}

	else if (Look_for("/led2off", buftocopyinto) == 1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
		Led2.off = 0x1;
		Server_Handle("/led2off",Link_ID);
	}

	else if (Look_for("/favicon.ico", buftocopyinto) == 1);

	else
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
		Led1.off = 0x1;
		Led2.off = 0x1;
		Server_Handle("/ ", Link_ID);
	}
}
