This Tutorial is based on https://controllerstech.com/send-and-receive-data-from-the-webserver-using-stm32

Here, we will switch 2 LEDs ON / OFF per WiFi

Circuit Diagram:
![image](https://github.com/user-attachments/assets/0a4f1a71-45db-4fe4-bc88-d5f7945c5b46)

Needed HW:
- STM32F103C8T (Bluepill)
- ESP8266 Wifi
- USB-Uart to PC
- ST-Link V2

Short Overview:
- We use for UART Communication Uart_isr because we use ringbuffer
- We have a Server on STM32
