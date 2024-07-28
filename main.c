/**
 * @file main.c
 * @author Abrahim Toutoungi, Liam Gossman, Drew Jensen, Andrew McMahon
 * Final Project main file for CprE 288
 */

#include <uart.h>
#include <lcd.h>
#include <ping.h>
#include <Timer.h>
#include <servo.h>
#include <button.h>
#include <adc.h>
#include <button.h>
#include <open_interface.h>
#include <movement.h>
#include <final_project_help.h>
#include <music.h>
#include <imu.h>
#include <bno055.h>

int main(void)
{
    // Allocate memory for the sensor data structure. This should only be done once at the start of main().
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);

    // Initialize the timer.
    timer_init();

    // Initialize the LCD.
    lcd_init();

    // Initialize UART.
    uart_init();

    // Initialize the ADC.
    adc_init();

    // Initialize the ping sensor.
    ping_init();

    // Initialize the buttons.
    button_init();

    // Set the current position to 2.
    set_current_pos();

    //Load the songs onto the bot
    load_songs();

    // IMU calibration
    bno_calib_t bno_calibration = { 0x000B, 0x0002, 0xFFF7, 0x03EC, 0xFE52,
                                    0xF8B5, 0xFFFE, 0xFFFE, 0xFFFF, 0x03E8,
                                    0x02EF };
    bno_t *bno = bno_alloc();
    bno_initCalib(&bno_calibration);

    // Initialize the servo.
    servo_init();

    // Set the servo calibration numbers.
    // To calibrate, comment out the following two lines and uncomment the following three lines.
    // Record the numbers under `servo_left_val` and `servo_right_val`.
    // Then, comment out the following three lines and uncomment the first two lines.
    //servo_get_calibrate();
    uint32_t servo_left_val = 284109;
    uint32_t servo_right_val = 312170;
    servo_set_calibrate(servo_right_val, servo_left_val);

    // Print "Ready" on the LCD to indicate that all the initialization steps have been completed.
    lcd_printf("Ready");

    // Send "Ready" over UART.
    uart_sendStr("\n\r");
    uart_sendStr("\n\rReady");

    // Initialize a variable to keep track of whether or not the robot is done.
    int done = 0;

    //Set a heading for straight
    bno_update(bno);
    heading = bno->euler.heading / 16.;


    // While the robot is not done, do the following:
    while (done != 1)
    {
        // Update the sensor data.
        oi_update(sensor_data);

        // Pull out of the starting spot and begin the detection process.
        face_forward_only(sensor_data, bno);
        turn_left(sensor_data, 90);
        move_forward_cliff(sensor_data, 600, bno);

        // Move the robot around the parking lot, scanning each spot and sending the full/empty status to the UART.
        face_forward(sensor_data, bno);

        // Set the `done` variable to 1 to indicate that the robot is done.
        done = 1;
    }

    // Free the memory for the sensor data structure.
    oi_free(sensor_data);

    // Return 0 to indicate that the program terminated successfully.
    return 0;
}
