/*
*
*   uart.c
*
*
*
*   @author Abrahim Toutoungi
*   @date
*/

#include <inc/tm4c123gh6pm.h>
#include <uart.h>
#include <stdint.h>
#include <Timer.h>
#include <string.h>
//#include "button.h"

void uart_init(void){
  //enable clock to GPIO port B
  SYSCTL_RCGCGPIO_R |= 0x02;

  //enable clock to UART1
  SYSCTL_RCGCUART_R |= 0x02;

  //wait for GPIOB and UART1 peripherals to be ready
  while ((SYSCTL_PRGPIO_R & 0x02) == 0) {};
  while ((SYSCTL_PRUART_R & 0x02) == 0) {};

  //enable alternate functions on port B pins
  GPIO_PORTB_AFSEL_R |= 0x3;

  //enable digital functionality on port B pins
  GPIO_PORTB_DEN_R |= 0x3;

  //enable UART1 Rx and Tx on port B pins
  GPIO_PORTB_PCTL_R |= 0x00000011;
  GPIO_PORTB_PCTL_R &= 0xFFFFFF11;

//  GPIO_PORTB_DIR_R &= 0xFE;
//  GPIO_PORTB_DIR_R |= 0x02;

  //calculate baud rate
  uint16_t iBRD = 16000000 / (16 * 115200); //use equations
  uint16_t fBRD = ((16000000.0 / (16 * 115200)) - iBRD) * 64 + 0.5; //use equations

  //turn off UART1 while setting it up
  UART1_CTL_R &= 0xFFFE;

  //set baud rate
  //note: to take effect, there must be a write to LCRH after these assignments
  UART1_IBRD_R = iBRD;
  UART1_FBRD_R = fBRD;

  //set frame, 8 data bits, 1 stop bit, no parity, no FIFO
  //note: this write to LCRH must be after the BRD assignments
  UART1_LCRH_R = 0x60;

  //use system clock as source
  //note from the datasheet UARTCCC register description:
  //field is 0 (system clock) by default on reset
  //Good to be explicit in your code
  UART1_CC_R = 0x0;

  //re-enable UART1 and also enable RX, TX (three bits)
  //note from the datasheet UARTCTL register description:
  //RX and TX are enabled by default on reset
  //Good to be explicit in your code
  //Be careful to not clear RX and TX enable bits
  //(either preserve if already set or set them)
  UART1_CTL_R |= 0x0301;

}

void uart_sendChar(char data){
    while (UART1_FR_R & 0x20) {
        if (UART1_FR_R == 0) {
            break;
        }
    }

    UART1_DR_R = data;
}

char uart_receive(void){
    char data = 0;

    while (UART1_FR_R & UART_FR_RXFE) {
        if (UART1_FR_R == 0) {
            break;
        }
    }

    data = (char) (UART1_DR_R & 0xFF);

    return data;

}

char uart_receive_nonblocking(void){
    char data = 0;


    if ((UART1_FR_R & UART_FR_RXFE) == 0) {
        data = (char) (UART1_DR_R & 0xFF);
    }


    return data;

}


void uart_sendStr(const char* data){
    int i = 0;
    while (data[i] != '\0') {
        uart_sendChar(data[i]);
        i++;
    }
}

void uart_sendStatus(int fullCarSpots, int fullMotoSpots, int emptySpots){
  uart_sendStr("\n\rFull (Car) Spots: ");
  char str[10];
  sprintf(str, "%d", fullCarSpots);
  uart_sendStr(str);
  uart_sendStr("\n\rFull (Moto) Spots: ");
  char str3[10];
  sprintf(str3, "%d", fullMotoSpots);
  uart_sendStr(str3);
  uart_sendStr("\n\rEmpty spots: ");
  char str2[10];
  sprintf(str2, "%d", emptySpots);
  uart_sendStr(str2);
  uart_sendStr("\n\r");
}

void uart_sendParkingLot(char parking2[7], char bot2[7], char bot1[7], char parking1[7]) {
    int i;
    int j;
    int k;

    char edge[7] = {'|', '|' ,'|' ,'|' ,'|' ,'|' ,'|'};

    uart_sendStr("\n\r");
    for (j = 0; j < 11; ++j) {
        uart_sendChar('-');
    }
    uart_sendStr("\n\r");
    for (i = 0; i < 7; ++i) {
        uart_sendChar(edge[i]);
        uart_sendChar(' ');
        uart_sendChar(parking2[i]);
        uart_sendChar(' ');
        uart_sendChar(bot2[i]);
        uart_sendChar(' ');
        uart_sendChar(bot1[i]);
        uart_sendChar(' ');
        uart_sendChar(parking1[i]);
        uart_sendChar(' ');
        uart_sendChar(edge[i]);
        uart_sendStr("\n\r");
        timer_waitMillis(100);
    }
    for (k = 0; k < 11; ++k) {
        uart_sendChar('-');
    }

    uart_sendStr("\n\r Key: \n\r H: Home \n\r E: Empty Spot \n\r C: Car in spot \n\r M: Motorcycle in spot \n\r P: Pot hole blocking spot \n\r R: Road within parking lot");
}



