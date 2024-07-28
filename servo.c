/*
 * servo.c
 *
 *  Created on: Apr 5, 2023
 *      Author: lgossman
 */

#include "servo.h"
#include "lcd.h"
#include "button.h"

float globalSlope;
uint32_t globalOffset;

void servo_init(void){

    SYSCTL_RCGCGPIO_R |= 0x02;
    SYSCTL_RCGCTIMER_R |= 0x02;
    while((SYSCTL_PRGPIO_R & 0x02) != 0x02){};
    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AFSEL_R |= 0x20;
    GPIO_PORTB_DEN_R |= 0x20;
    GPIO_PORTB_PCTL_R |= 0x00700000;

    TIMER1_CTL_R &= ~0x100;
    TIMER1_CFG_R |= 0x4;
    TIMER1_TBMR_R = 0x0A;
    TIMER1_CTL_R &= ~0x4000;
    TIMER1_TBPR_R = 0x04;
    TIMER1_TBILR_R = 0xE200;
    TIMER1_TBPMR_R = 0x4;
    TIMER1_TBMATCHR_R = 0x903B;
    TIMER1_CTL_R |= 0x100;

    globalSlope = 88.8;
    globalOffset = 16000;

}

void servo_move(long degrees){
    long long clockCycles;
    uint32_t matchCycles;
    clockCycles = globalSlope * degrees + globalOffset;
    matchCycles = 320000 - clockCycles;
    TIMER1_TBMATCHR_R = matchCycles & 0xFFFF;
    //lcd_printf("%u", matchCycles);
    matchCycles = matchCycles >> 16;
    TIMER1_TBPMR_R = matchCycles & 0xF;

}

void servo_set_calibrate(uint32_t right_number, uint32_t left_number){

    globalOffset = 320000 - right_number;
    globalSlope = (320000 - left_number - globalOffset)/180;

}

void servo_get_calibrate(void){

    int direction = 0; // 0 = clockwise, 1 = counterclockwise
    int angle = 90;
    lcd_printf("Clockwise");

    while(1){

        if(button_getButton() == 1){
            if(direction == 0){
                angle--;
                servo_move(angle);
            }
            else if(direction == 1){
                angle++;
                servo_move(angle);
            }
            timer_waitMillis(250);
        }
        else if(button_getButton() == 2){
            if(direction == 0){
                angle -= 5;
                servo_move(angle);
            }
            else if(direction == 1){
                angle += 5;
                servo_move(angle);
            }
            timer_waitMillis(250);
        }
        else if(button_getButton() == 3){
            if(direction == 0){
                direction = 1;
                lcd_printf("Counterclockwise");
            }
            else if(direction == 1){
                direction = 0;
                lcd_printf("Clockwise");
            }
            timer_waitMillis(250);
        }
        else if(button_getButton() == 4){
            if(direction == 0){
                angle = 5;
                servo_move(angle);
            }
            else if(direction == 1){
                angle = 175;
                servo_move(angle);
            }
            timer_waitMillis(250);
        }
    }
}
