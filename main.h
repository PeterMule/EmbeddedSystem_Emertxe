/* 
 * File:   main.h
 * Author: PETER
 *
 * Created on March 23, 2022, 4:54 PM
 */

#ifndef MAIN_H
#define	MAIN_H

#include <xc.h>
#include <string.h>
#include <pic.h>
#include "adc.h"
#include "clcd.h"
#include "digital_keypad.h"
#include "ds1307.h"
#include "i2c.h"
#include "car_black_box_de.h"
#include "timers.h" 

#define LOGIN_SUCCESS           0xB0
#define RETURN_BACK             0xB1


#define CURSOR_POS              6
#define TASK_SUCCESS            0xA1
#define TASK_FAIL               0xA2

#define DASHBOARD_FLAG          1
#define LOGIN_FLAG              2
#define MENU_FLAG               3
#define VIEW_LOGS_FLAG          4
#define CLEAR_LOGS_FLAG         5
#define CHANGE_PASS_FLAG        6

#define RESET_PASSWORD          0xF0
#define RESET_NOTHING           0xF1
#define RESET_LOGIN_MENU        0xF2
#define RESET_CLEAR_LOGS        0xF3
#define RESET_CHANGE_PASSWORD   0xF4

#define PASS_SW4                '0'
#define PASS_SW5                '1'


#endif	/* MAIN_H */

