/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * This is a bare minimum user application template.
 *
 * For documentation of the board, go \ref group_common_boards "here" for a link
 * to the board-specific documentation.
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# Basic usage of on-board LED and button
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include <stdio.h>
#include "lcd.h"


#define CONF_USART_MUX_SETTING USART_RX_1_TX_0_XCK_1 
#define CONF_USART_PINMUX_PAD0 PINMUX_PA14D_SERCOM2_PAD0
#define CONF_USART_PINMUX_PAD1 PINMUX_PA15D_SERCOM2_PAD1
#define CONF_USART_PINMUX_PAD2 PINMUX_UNUSED
#define CONF_USART_PINMUX_PAD3 PINMUX_UNUSED
#define CONF_USART_MODULE SERCOM2


// UART Uses SERCOM2 PA10 and PA11
// http://asf.atmel.com/docs/latest/samd21/html/asfdoc_sam0_sercom_usart_mux_settings.html
struct usart_module usart_instance;

// void configure_usart(void)
// {
// 	struct usart_config config_usart;
// 	usart_get_config_defaults(&config_usart);
// 	config_usart.baudrate    = 9600;
// 	config_usart.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
// 	config_usart.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0; // unused
// 	config_usart.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1; // unused
// 	config_usart.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2; // PA10
// 	config_usart.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3; // PA11
// 	while (usart_init(&usart_instance,
// 	EDBG_CDC_MODULE, &config_usart) != STATUS_OK) {
// 	}
// 	usart_enable(&usart_instance);
// }

void configure_usart(void)
{
	struct usart_config config_usart;
	usart_get_config_defaults(&config_usart);
	config_usart.baudrate    = 9600;
	config_usart.mux_setting = CONF_USART_MUX_SETTING;
	config_usart.pinmux_pad0 = CONF_USART_PINMUX_PAD0; // TX
	config_usart.pinmux_pad1 = CONF_USART_PINMUX_PAD1; // RX
	config_usart.pinmux_pad2 = CONF_USART_PINMUX_PAD2; // unused
	config_usart.pinmux_pad3 = CONF_USART_PINMUX_PAD3; // unused
	while (usart_init(&usart_instance,
	CONF_USART_MODULE, &config_usart) != STATUS_OK) {
	}
	usart_enable(&usart_instance);
}


void process_numbers(int* num_array, char* buffer, char* charBuf){
	int j = 0,i=0;
	while(buffer[i]!=')'){
		if(buffer[i]==','){
			j++;
			if(j==5){
				i++;
				int k = 0;
				while(buffer[i]!=')'){
					charBuf[k]=buffer[i];
					i++;
					k++;
				}
				break;				
			}
		}
// 		if(j==5){ // if it is a string based buffer
// 			if (buffer[0]==3){ //store the first four numbers that define it, and then store the rest in a charbuffer
// 				i++;
// 				int k = 0;
// 				while(buffer[i]!=')'){
// 					charBuf[k]=buffer[i];
// 					i++;
// 					k++;
// 				}
// 				break;
// 			}
// 		}
		else{
			num_array[j]=num_array[j]*10+buffer[i]-'0';
		}
		i++;
	}
}

int main (void)
{
	//uint16_t txBuf[20];
	uint8_t rxBuf[50];
	char charBuf[50];
	system_init();
	delay_init();
	configure_usart();
 	configure_port_pins();
	configure_spi_master();
	LCD_init();
// 	LCD_clearScreen(BLUE);
// 	delay_s(1);
// 	LCD_clearScreen(RED);
// 	delay_s(1);
// 	LCD_clearScreen(GREEN);
// 	delay_s(1);
	LCD_clearScreen(BLACK);
	uint8_t string[] = "Hello World!\r\n";
	usart_write_buffer_wait(&usart_instance, string, sizeof(string));
	uint16_t temp;
	uint16_t counter;
	counter = 0;
	int number_array[20];
	while (true) {
		// Need to write some kind of buffer storage..
// 		if (usart_read_wait(&usart_instance, &temp) == STATUS_OK) {
// 			 				    while (usart_write_wait(&usart_instance, temp) != STATUS_OK) {
// 			 				    }
// 			 			    }
		while(1){
			if (counter>50){
				counter = 0;
			}
			while(usart_read_wait(&usart_instance, &temp)!=STATUS_OK){;}
			//drawChar(0,0,(char)temp,WHITE,BLACK);
			rxBuf[counter]=(uint8_t) temp;
			if(rxBuf[counter]==')'){  
				// we can read information here
				
					// list of commands
				process_numbers(&number_array,&rxBuf,&charBuf);
				switch(number_array[0]){
					case 1:
						LCD_drawXLine(number_array[1],number_array[2]);
						break;
					case 2:
						LCD_drawYLine(number_array[1],number_array[2]);
						break;
					case 3:
						drawChar(number_array[1], number_array[2], (unsigned char)charBuf[0], number_array[3], number_array[4]);
						break;
					case 4:
						drawString(number_array[1], number_array[2], &charBuf, number_array[3], number_array[4]);
						break;
					case 5:
						LCD_clearScreen(number_array[1]);
						break;
					case 6:
						LCD_drawPixel(number_array[1],number_array[2],number_array[3]);
						
				}
				//number_array[0]=1;
				//number_array[1]=2;
				//drawChar(0,20,number_array[0]+'0',WHITE,BLACK);
				//drawChar(0,30,number_array[1]+'0',WHITE,BLACK);
				memset(number_array,0,sizeof(number_array)); // change everything to zero.
				memset(charBuf,0,sizeof(charBuf));
				usart_write_buffer_wait(&usart_instance, &rxBuf,counter);
				memset(rxBuf,0,sizeof(rxBuf));
// 				for(int i = 0;i<2;i++){
// 					int somenum = 10+10*i;
// 					drawChar(0,somenum,number_array[i],WHITE,BLACK);
// 				}
				
				
				//drawString(0,10,&rxBuf,WHITE,BLACK);
				counter = 0;
			}
			else{
				counter++;
			}
		}

			
		//sprintf(txBuf,"some var\r\n");
		//usart_write_buffer_wait(&usart_instance, &txBuf, 20);
		//delay_s(1);
	}

	/* Insert application code here, after the board has been initialized. */

	/* This skeleton code simply sets the LED to the state of the button. */
	while (1) {
		
		/* Is button pressed? */
		if (port_pin_get_input_level(BUTTON_0_PIN) == BUTTON_0_ACTIVE) {
			/* Yes, so turn LED on. */
			port_pin_set_output_level(LED_0_PIN, LED_0_ACTIVE);
		} else {
			/* No, so turn LED off. */
			port_pin_set_output_level(LED_0_PIN, !LED_0_ACTIVE);
		}
	}
}
