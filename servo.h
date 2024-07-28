/*
 * servo.h
 *
 *  Created on: Apr 5, 2023
 *      Author: lgossman
 */

#ifndef SERVO_H_
#define SERVO_H_

#include <inc/tm4c123gh6pm.h>
#include <stdbool.h>
#include <stdint.h>
#include "driverlib/interrupt.h"

void servo_init(void);

void servo_move(long degrees);

void servo_set_calibrate(uint32_t right_number, uint32_t left_number);

void servo_get_calibrate(void);

#endif /* SERVO_H_ */
