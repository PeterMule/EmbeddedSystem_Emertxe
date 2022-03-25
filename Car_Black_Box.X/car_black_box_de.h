/* 
 * File:   car_black_box_de.h
 * Author: PETER
 *
 * Created on March 23, 2022, 5:09 PM
 */

#ifndef CAR_BLACK_BOX_DE_H
#define	CAR_BLACK_BOX_DE_H
#include "main.h"

unsigned char get_speed(void);
void display_time();
void display_dashboard(char*,unsigned char);
char display_log_car_event(char);
unsigned char* read_log_car_event(char index);
void log_car_event(char event[], unsigned char speed);

void login(unsigned char key,unsigned char reset_flag);
void clcd_clear_screen(void);
#endif	/* CAR_BLACK_BOX_DE_H */

