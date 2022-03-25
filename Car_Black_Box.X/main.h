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



#define CURSOR_POS              6
#define DASHBOARD_FLAG          1
#define LOGIN_FLAG              2
#define VIEW_LOGS_FLAG          3
#define RESET_PASSWORD          0x01
#define RESET_NOTHING           0xF1
#define PASS_SW4                '0'
#define PASS_SW5                '1'


unsigned char sec;
#endif	/* MAIN_H */

