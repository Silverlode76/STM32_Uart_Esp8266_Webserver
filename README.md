This Tutorial is based on https://controllerstech.com/send-and-receive-data-from-the-webserver-using-stm32

THANKS A LOT FOR SUCH A GREAT TUTORIAL

Here, we will switch 2 LEDs ON / OFF per WiFi

Circuit Diagram:
![image](https://github.com/user-attachments/assets/b5d3187d-853c-461e-b8bb-16c9e3f9b0af)


Needed HW:
- STM32F103C8T (Bluepill)
- ESP8266 Wifi
- USB-Uart to PC
- ST-Link V2

Short Overview:
- We use for UART Communication Uart_isr because we use ringbuffer
- We have a Server on STM32 which generates the Webpage

Configuration:

![image](https://github.com/user-attachments/assets/4d35f2a2-3596-489b-9ca8-4875d0e4a408)

- UART3 goes to WiFI
- UART1 goe to PC
- For both UARTS we use ASYNCHONOUS and global Interrupt enable. Bausrate is set to 115200 Bauds

![image](https://github.com/user-attachments/assets/197156ff-af20-4413-a41e-278537edf550)

![image](https://github.com/user-attachments/assets/9385207c-2ad2-4d63-8f87-1507e58c733b)



Functions:

- in "ESP8266_HAL.c" assign your uarts

![image](https://github.com/user-attachments/assets/4e4d2db0-e570-4857-ad56-375d525856a5)

- in "stm32f1xx_it.c" put isr routines ans comment standard IRQHandler

![image](https://github.com/user-attachments/assets/ced4dc5c-10d9-479e-922f-75540cbb6b1b)

- in "main.c" function ESP_Init() is trying to access your WLAN-AP. YOu have to put here your W-Lan and Passwd

![image](https://github.com/user-attachments/assets/ba7bd416-bc44-449e-a6b9-7ffa3caebe2b)

Checks:
- If everything works fine you should see in PC UART that we have an access point

![image](https://github.com/user-attachments/assets/85545ee9-4028-4169-bb54-1a47182e7c58)

- Open in the Browser the IP and you will see follwing screen

![image](https://github.com/user-attachments/assets/b2a59726-99eb-4bab-b782-dfe37bbf2a97)



