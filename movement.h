/*
 * movement.h
 *
 *  Created on: April 25, 2023
 *      Author: Abrahim Toutoungi
 */

#ifndef LAB7_CODE_FILES_MOVEMENT_H_
#define LAB7_CODE_FILES_MOVEMENT_H_

#include "open_interface.h"

double move_backward (oi_t *sensor_data, double distance_mm);
double turn_right(oi_t *sensor, double degrees);
double turn_left(oi_t  *sensor, double degrees);
double move_forward (oi_t *sensor_data, double distance_mm);
//void turn_right_imu(oi_t *sensor_data, double degrees);
void turn_left_imu(oi_t *sensor_data, double degrees);



#endif /* LAB7_CODE_FILES_MOVEMENT_H_ */
