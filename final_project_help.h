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
#include <bno055.h>

void set_current_pos(void);
void move_forward_cliff(oi_t *sensor_data, float dis, bno_t *bno);
void scan_spot(oi_t *sensor_data, bno_t *bno);
void move_spot(oi_t *sensor_data, bno_t *bno);
int get_current_pos(void);
void move_loop(oi_t * sensor_data, bno_t *bno);
void face_forward(oi_t *sensor_data, bno_t *bno);
int get_cars(void);
int get_motos(void);
int get_empty(void);
void face_forward_only(oi_t *sensor_data, bno_t *bno);

extern float heading;
