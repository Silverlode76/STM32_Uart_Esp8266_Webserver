/*
 * ESP8266_HAL.h
 *
 *  Created on: Apr 14, 2020
 *      Author: Controllerstech
 */

#ifndef INC_ESP8266_HAL_H_
#define INC_ESP8266_HAL_H_

typedef struct
{
  unsigned char off;
} Led_state;

void ESP_Init (char *SSID, char *PASSWD);

void Server_Start (void);


#endif /* INC_ESP8266_HAL_H_ */
