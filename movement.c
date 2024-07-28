/*
 * movement.c
 *
 *  Created on: April 25, 2023
 *      Author: Abrahim Toutoungi
 */


#include "lcd.h"
#include "open_interface.h"
#include "servo.h"
#include "ping.h"
#include "adc.h"
#include "uart.h"
#include "imu.h"
#include "bno055.h"

//bno_calib_t bno_calibration = {0x000B, 0x0002, 0xFFF7, 0x03EC, 0xFE52, 0xF8B5,
//                               0xFFFE, 0xFFFE, 0xFFFF, 0x03E8, 0x02EF };


void move_backward (oi_t *sensor_data, double distance_mm){

    lcd_init();

    // the following code could be put in function move_forward()
    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(-100, -100); //move forward at full speed
    while (sum > distance_mm)
    {
        oi_update(sensor_data);
        sum += sensor_data->distance; // use -> notation since pointer
    }
    oi_setWheels(0, 0); //stop

}


void turn_right(oi_t *sensor, double degrees){

    double sum = 0;
    oi_setWheels(-69, 69);
    while(sum > -degrees){
        oi_update(sensor);
        sum += sensor->angle;
    }
    oi_setWheels(0, 0);
}

void turn_left(oi_t *sensor_data, double degrees) {

    double sum = 0;
    oi_setWheels(69,-69);
    while (sum < degrees) {
        oi_update(sensor_data);
        sum += sensor_data -> angle;
    }
    oi_setWheels(0,0);
}


void turn_left_imu(oi_t *sensor_data, double degrees){
//TODO: IMU
//    // Get initial heading
//    double initial_heading;
//    mag_t *mag = imu_getMag();
//    initial_heading = mag->heading;
//    free(mag);
//
    // Turn left
    turn_left(sensor_data, degrees);
//
//    // Get final heading
//    double final_heading;
//    mag = imu_getMag();
//    final_heading = mag->heading;
//    free(mag);
//
//    // Check that the robot turned the correct amount
//    double actual_turn = final_heading - initial_heading;
//    double remainder = degrees - actual_turn;
//    while (fabs(actual_turn - degrees) > 1) {
//            if(remainder < 0){
//                turn_right_imu(sensor_data, remainder);
//            } else if(remainder > 0){
//                turn_left_imu(sensor_data, remainder);
//            }
//    }
}


void turn_right_imu(oi_t *sensor_data, double degrees){
//TODO: IMU
    // IMU calibration
    bno_calib_t bno_calibration = {0x000B, 0x0002, 0xFFF7, 0x03EC, 0xFE52, 0xF8B5,
                                   0xFFFE, 0xFFFE, 0xFFFF, 0x03E8, 0x02EF };
    bno_t *bno = bno_alloc();
    bno_initCalib(&bno_calibration);

    bno_update(bno);
    float target = bno->euler.heading / 16.;
//    turn_right(sensor_data, 90);
    while(1){

        oi_update(sensor_data);

        if (sensor_data->wheelDropLeft || sensor_data->wheelDropRight) {
            oi_setWheels(0, 0);
        }
        else {
            bno_update(bno);
            lcd_printf("%f", target);
            float hi = target + 90.0;
            float difference = remainderf(
                    (hi - (bno->euler.heading / 16.) + 360), 360);
            difference = difference > 180 ? difference - 360 : difference;
            difference *= 14;

            int16_t power = (int16_t) (
                    difference < -100 ?
                            -100 : (difference > 100 ? 100 : difference));

            oi_setWheels(-power/2, power/2);
        }
        timer_waitMillis(10);
    }
}

void move_forward (oi_t *sensor_data, double distance_mm){

    lcd_init();

    // the following code could be put in function move_forward()
    double sum = 0; // distance member in oi_t struct is type double
    oi_setWheels(75, 68); //move forward at full speed
    while (sum < distance_mm)
    {
        oi_update(sensor_data);
        sum += sensor_data->distance; // use -> notation since pointer
        //lcd_printf("Distance: %lf", sum);

        if(sensor_data->bumpRight){
            oi_setWheels(0, 0);
            move_backward(sensor_data, -7);
            //distance_mm += 75;
            turn_left(sensor_data, 40);
            move_forward(sensor_data, 165);
            turn_right(sensor_data, -65);
            move_forward(sensor_data, 7);
            move_forward(sensor_data, distance_mm - sum);
        }
        else if(sensor_data->bumpLeft){
            oi_setWheels(0, 0);
            move_backward(sensor_data, -7);
            //distance_mm += 75;
            turn_right(sensor_data, -40);
            move_forward(sensor_data, 165);
            turn_left(sensor_data, 65);
            move_forward(sensor_data, 7);
            move_forward(sensor_data, distance_mm - sum);
        }

    }
    oi_setWheels(0, 0); //stop

}


