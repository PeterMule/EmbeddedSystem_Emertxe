/* 
 * File:   main.c
 * Author: PETER
 *
 * Created on March 23, 2022, 4:49 PM
 */

#include "main.h"

#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)

static void init_config(void)
{
    //
    init_clcd();
    //
    //init_digital_keypad();
    //
    init_adc();
    //
    //init_ds1307();
    //
    init_i2c(100000);//100k
    
}

void main(void) {
    char event[3] = "ON";
    unsigned char speed = 0;
    
    unsigned char control_flag = 1;
    
    init_config();
    
    while(1)
    {
        switch(control_flag)
        {
            case 1: //Dashboard
                speed = ((float)read_adc()/1023 * 99);
                display_dashboard(event,speed);
                break;
            default:
                
                break;
        }
    }
    return;
}
