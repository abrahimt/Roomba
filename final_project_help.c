/**
 * @file final_project_help.c
 * @author Abrahim Toutoungi, Liam Gossman, Drew Jensen, Andrew McMahon
 * Final Project helper file for CprE 288
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

bno_calib_t bno_calibration1 = { 0x000B, 0x0002, 0xFFF7, 0x03EC, 0xFE52, 0xF8B5,
                                 0xFFFE, 0xFFFE, 0xFFFF, 0x03E8, 0x02EF };

// Heading
float heading;

// This variable stores the bot's current position.
int currentPos;

// This array stores the table positions.
char parkingCol1[7] = { 'P', 'P', 'P', 'P', 'P', 'P', 'H' };
char parkingCol2[7] = { 'P', 'P' ,'P' ,'P' ,'P','P' ,'P' };
char botCol1[7] = { 'R', 'R' ,'R','R', 'R', 'R', 'R' };
char botCol2[7] = { 'R', 'R' ,'R','R', 'R', 'R', 'R' };

// Stores the number of car, motorcycles, and empty spots
int fullCarSpots = 0;
int fullMotoSpots = 0;
int emptySpots = 0;

// flag to change things when bot turns around at the end of the parking lot
int flag = 0;

// This variable stores the right turn angle.
int right_angle = 86;

// This variable stores the left turn angle.
int left_angle = 80;

// This function sets the current position to 2.
void set_current_pos(void)
{
    currentPos = 2; // The bot will technically only start scanning from the second tile.
}

// This function returns the current position.
int get_current_pos(void)
{
    return currentPos; // Returns what tile the bot is currently on.
}

int get_cars(void) {
  // This function returns the number of cars that are currently parked.
  return fullCarSpots;
}

int get_motos(void) {
  // This function returns the number of motorcycles that are currently parked.
  return fullMotoSpots;
}

int get_empty(void) {
  // This function returns the number of empty parking spots.
  return emptySpots;
}


// This function moves the robot forward, scans the spot, and returns the status of the spot.
void move_spot(oi_t *sensor_data, bno_t *bno)
{
    oi_update(sensor_data);
    bno_update(bno);
    // Move the robot forward until it reaches the next spot. (One tile is approximately 60 cm or 600 mm)
    move_forward_cliff(sensor_data, 600, bno);

    // Scan the spot.
    scan_spot(sensor_data, bno);
}

// Move the robot forward for a given distance while checking for obstacles(in millimeters).
void move_forward_cliff(oi_t *sensor_data, float distance_mm, bno_t *bno)
{
    double sum = 0;

    // Move the robot forward at full speed until the desired distance is reached.
    oi_setWheels(65, 65);
    while (sum < distance_mm)
    {
        oi_update(sensor_data);
        sum += sensor_data->distance; // Use pointer notation since `sensor_data` is a pointer.

        if (sensor_data->bumpRight || sensor_data->bumpLeft)
        { // go home
          // Play song while driving around because it is cool.
            oi_play_song(0);
            move_backward(sensor_data, -120);
            turn_left(sensor_data, left_angle + 2);
            move_forward(sensor_data, 1150);
            turn_left(sensor_data, left_angle);
            // Send final array
            uart_sendParkingLot(parkingCol2, botCol2, botCol1, parkingCol1);
        }

        // If the bot hits the boundary, turn around and scan the spot on the other side.
        if (sensor_data->cliffFrontLeftSignal > 2700
                || sensor_data->cliffFrontRightSignal > 2700
                || sensor_data->cliffLeftSignal > 2700
                || sensor_data->cliffRightSignal > 2700)
        { // turn around
            flag = 1;

            move_backward(sensor_data, -150);
            turn_left(sensor_data, left_angle + 4);
            face_forward_only(sensor_data, bno);
            turn_left(sensor_data, left_angle + 4);
            move_forward_cliff(sensor_data, 600, bno);

            // Change heading so that it is now the other way
            heading += 180.0;

            face_forward_only(sensor_data, bno);
            timer_waitMillis(1000);
            scan_spot(sensor_data, bno);
            move_loop(sensor_data, bno);
        }
        // If the robot detects a cliff, perform a specific set of movements depending on the current position of the robot.
        if (sensor_data->cliffFrontLeftSignal < 20
                || sensor_data->cliffFrontRightSignal < 20
                || sensor_data->cliffLeftSignal < 20
                || sensor_data->cliffRightSignal < 20)
        {
            lcd_printf("Hole in the way!");
            oi_setWheels(0, 0);
            timer_waitMillis(1000);

            // CASE 1 (most likely to occur): Tiles 2, 3, 4, 5, 6, 9, 10, 11, 12
            if (currentPos == 2 || currentPos == 3 || currentPos == 4
                    || currentPos == 5 || currentPos == 6 || currentPos == 9
                    || currentPos == 10 || currentPos == 11 || currentPos == 12)
            {
            currentPos++;

            move_backward(sensor_data, -220);
            turn_left(sensor_data, left_angle);
            move_forward(sensor_data, 610);

            // Make sure it is facing the heading
            face_forward_only(sensor_data, bno);

            move_forward(sensor_data, 1300);
            turn_right(sensor_data, right_angle - 2);
            move_forward(sensor_data, 600);
            face_forward_only(sensor_data, bno);
            timer_waitMillis(1000);
            scan_spot(sensor_data, bno);
            move_loop(sensor_data, bno);
            }
            // CASE 2: Tile 7
            else if (currentPos == 7)
            {
                move_backward(sensor_data, -220);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, right_angle);
                move_forward(sensor_data, 600);
                turn_left(sensor_data, 175);
                scan_spot(sensor_data, bno);
                turn_left(sensor_data, left_angle);
                currentPos++;
                move_loop(sensor_data, bno);
            }
            //CASE 3 (Tiles: 8):
            else if (currentPos == 8)
            {
                move_backward(sensor_data, -220);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, right_angle);
                move_forward(sensor_data, 600);
                turn_left(sensor_data, left_angle);
                scan_spot(sensor_data, bno);
                turn_left(sensor_data, left_angle);
                currentPos++;
                move_loop(sensor_data, bno);
            }
            //CASE 4 (Tiles: 13):
            else if (currentPos == 13)
            {
                move_backward(sensor_data, -220);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, right_angle);
                move_forward(sensor_data, 1300);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, 180);
                break; //bot would be home so it should end the program (call uart final message to send)
            }
            //CASE 5 (Tiles: 14):
            else if (currentPos == 14)
            {
                move_backward(sensor_data, -220);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, right_angle);
                move_forward(sensor_data, 600);
                turn_left(sensor_data, left_angle);
                move_forward(sensor_data, 600);
                turn_right(sensor_data, 180);
                break; //bot would be home so it should end the program (call uart final message to send)
            }
        }
    }

    oi_setWheels(0, 0); //stop

}

// This function scans a spot for objects.
void scan_spot(oi_t *sensor_data, bno_t *bno) {

  // Declare variables.
  uint16_t IR_val[20] = { 0 }; // Array to store the IR values.
  int pos = 0; // Index of the current IR value.
  int average = 0; // Average IR value.
  int i; // Loop counter.
  int j; // Loop counter.

  // Turn the bot right.
  turn_right(sensor_data, right_angle);

  // For each angle, measure the distance to the nearest object and the IR value.
  for (i = 70; i <= 110; i += 2) {

    // Move the servo to the current angle.
    servo_move(i);

    // Get the IR value.
    IR_val[pos] = adc_read();

    // Print the IR value to the LCD screen.
    lcd_printf("%u", IR_val[pos]);

    // Increment the position of the current IR value.
    pos += 1;

    // Wait 250 milliseconds.
    timer_waitMillis(250);
  }

  // Calculate the average IR value.
  for (j = 0; j < 20; j++) {

    // If the current IR value is not 0, add it to the average.
    if (IR_val[j] != 0) {
      average += IR_val[j];
    }
  }

  // Divide the sum of the IR values by 20 to get the average IR value.
  average /= 20;

  // Print the average IR value to the LCD screen.
  lcd_printf("%d", average);

  // If the average IR value is greater than 700, the spot is full with a motorcycle.
  if (average > 700 && average < 820) {

    // If the flag is 0, the bot is scanning the first column of parking spots.
    if (flag == 0) {

      // Set the current parking spot in the first column to be a motorcycle.
      parkingCol1[7 - currentPos] = 'M';
    }

    // Otherwise, the bot is scanning the second column of parking spots.
    else {

      // Set the current parking spot in the second column to be a motorcycle.
      parkingCol2[currentPos - 8] = 'M';
    }

    // Increment the number of full motorcycle spots.
    fullMotoSpots++;
  }

  // Otherwise, if the average IR value is greater than or equal to 820, the spot is full with a car.
  else if (average >= 820) {

    // If the flag is 0, the bot is scanning the first column of parking spots.
    if (flag == 0) {

      // Set the current parking spot in the first column to be a car.
      parkingCol1[7 - currentPos] = 'C';
    }

    // Otherwise, the bot is scanning the second column of parking spots.
    else {

      // Set the current parking spot in the second column to be a car.
      parkingCol2[currentPos - 8] = 'C';
    }

    // Increment the number of full car spots.
    fullCarSpots++;
  }

  // Otherwise, the spot is empty.
  else {

    // Increment the number of empty spots.
    emptySpots++;
  }

  // Turn the servo back to 90 degrees.
  servo_move(90);

  // Move the bot to the next spot.
  currentPos++;

  // Update the orientation data.
  bno_update(bno);

  // Update the sensor data.
  oi_update(sensor_data);

  // Move the bot to the next spot.
  move_loop(sensor_data, bno);
}


// This function moves the bot in a loop, scanning each spot and sending the full/empty status to the UART.
void move_loop(oi_t *sensor_data, bno_t *bno)
{
    oi_update(sensor_data);
    bno_update(bno);

// While the bot has not reached the last spot, scan the spot and move to the next spot.
    while (1)
    {
        lcd_printf("Current position: %d", currentPos);

        face_forward(sensor_data, bno);

    }
}

// This method is used within the loop to face the correct heading and then move forward and scan a spot
void face_forward(oi_t *sensor_data, bno_t *bno) {

  // Declare a variable to keep track of whether or not the robot has reached its desired heading.
  int done = 0;

  // While the robot has not reached its desired heading, do the following:
  while (done == 0) {

    // Update the sensor data and the orientation data.
    oi_update(sensor_data);
    bno_update(bno);

    // Calculate the difference between the desired heading and the current heading.
    // The difference is calculated in degrees, and it is wrapped around so that it is always between -180 and 180 degrees.
    float difference = remainderf(
        (heading - (bno->euler.heading / 16.) + 360), 360);

    // If the difference is greater than 180 degrees, subtract 360 degrees from it.
    difference = difference > 180 ? difference - 360 : difference;

    // Multiply the difference by 14 to scale it up.
    difference *= 14;

    // Print the difference to the LCD screen.
    lcd_printf("%f", difference);

    // Calculate the power that should be applied to each wheel.
    // The power is calculated based on the difference between the desired heading and the current heading.
    int16_t power =
        (int16_t) (
            difference < -100 ?
                -100 : (difference > 100 ? 100 : difference));

    // Apply the power to the wheels.
    oi_setWheels(-power * 3 / 4, power * 3 / 4);

    // If the difference is within 10 degrees of the desired heading, set the done flag to 1.
    if (difference < 10 && difference > -10) {
      done = 1;
    }
  }

  // Wait 10 milliseconds.
  timer_waitMillis(10);
}

void face_forward_only(oi_t *sensor_data, bno_t *bno) {

  // This method is used within the loop to face the correct heading only.
  // It does not move the robot forward.

  // Declare a variable to keep track of whether or not the robot has reached its desired heading.
  int done = 0;

  // While the robot has not reached its desired heading, do the following:
  while (done == 0) {

    // Update the sensor data and the orientation data.
    oi_update(sensor_data);
    bno_update(bno);

    // Calculate the difference between the desired heading and the current heading.
    // The difference is calculated in degrees, and it is wrapped around so that it is always between -180 and 180 degrees.
    float difference = remainderf(
        (heading - (bno->euler.heading / 16.) + 360), 360);

    // If the difference is greater than 180 degrees, subtract 360 degrees from it.
    difference = difference > 180 ? difference - 360 : difference;

    // Multiply the difference by 14 to scale it up.
    difference *= 14;

    // Print the difference to the LCD screen.
    lcd_printf("%f", difference);

    // Calculate the power that should be applied to each wheel.
    // The power is calculated based on the difference between the desired heading and the current heading.
    int16_t power =
        (int16_t) (
            difference < -100 ?
                -100 : (difference > 100 ? 100 : difference));

    // Apply the power to the wheels.
    // The wheels are turned in opposite directions so that the robot rotates around its center.
    oi_setWheels(-power * 3 / 4, power * 3 / 4);

    // If the difference is within 10 degrees of the desired heading, set the done flag to 1.
    if (difference < 10 && difference > -10) {
      done = 1;
    }
  }

  // Wait 10 milliseconds.
  timer_waitMillis(10);
}


