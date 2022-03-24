/* 
 * File:   main.c
 * Author: PETER
 *
 * Created on March 23, 2022, 4:49 PM
 */

#include "main.h"


#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)


unsigned char* gear[] = {"ON","C ", "GN","GR","G1","G2","G3","G4"};


static void init_config(void)
{
    //
    init_clcd();
    //
    init_digital_keypad();
    //
    init_adc();
    //
    init_i2c(100000);
    //
    init_ds1307();
    
}

void main(void) {
    
    init_config();
    
    eeprom_write(1,'A');
    unsigned char speed = get_speed();
    unsigned char key;
    unsigned char gr = 0;
    char event[3];
    strcpy(event,gear[gr]);
    log_car_event(event,speed);
    unsigned char control_flag = DASHBOARD_FLAG;
    char logs_index = 0;
    
    
    
    
    while(1)
    {
        key = read_digital_keypad(STATE);
        
        //Remove bouncing Effect
        for(int i = 0; i<30;i++);
        /*
        //SET Events 
        if(key == SW1)
        {
            gr = 1;
            strcpy(event,gear[gr]);
            speed = get_speed();
            log_car_event(event,speed);
        }
        else if(key == SW2 && gr < 8)
        {
            strcpy(event,gear[gr<2?gr=2:gr++]);
            speed = get_speed();
            log_car_event(event,speed);
        }
        else if(key == SW3 && gr > 2)
        {
            strcpy(event,gear[--gr]);
            speed = get_speed();
            log_car_event(event,speed);
        }
        else if(control_flag == DASHBOARD_FLAG && (key == SW4 || key == SW5))
        {
            clcd_clear_screen();
            clcd_print(" ENTER PASSWORD", LINE1(0));
            clcd_print(" ", LINE2(CURSOR_POS));
            clcd_write(CURSOR_POS, INST_MODE);
            clcd_write(DISP_ON_AND_CURSOR_ON,CURSOR_POS);
            control_flag = LOGIN_FLAG;
        }*/
        switch(control_flag)
        {
            case DASHBOARD_FLAG: //Dashboard
                switch(key)
                {
                    case SW1:
                    {
                        
                        gr = 1;
                        strcpy(event,gear[gr]);
                        speed = get_speed();
                        log_car_event(event,speed);
                        
                        display_dashboard(event,speed);
                        break;
                    }
                    case SW2:
                    {
                        if (gr < 8)
                        {
                            strcpy(event,gear[gr<2?gr=2:gr++]);
                            speed = get_speed();
                            log_car_event(event,speed);
                            
                            display_dashboard(event,speed);
                        }
                        break;
                    }
                    case SW3:
                    {
                        if (gr > 2)
                        {
                            strcpy(event,gear[--gr]);
                            speed = get_speed();
                            log_car_event(event,speed);
                            
                            display_dashboard(event,speed);
                        }
                        break;
                    }
                    case SW4:
                    case SW5:
                    {
                        clcd_clear_screen();
                        clcd_print(" ENTER PASSWORD", LINE1(0));
                        clcd_print(" ", LINE2(CURSOR_POS));
                        //clcd_write(CURSOR_POS, INST_MODE);
                        //clcd_write(DISP_ON_AND_CURSOR_ON,CURSOR_POS);
                        control_flag = LOGIN_FLAG;
                        break;
                    }
                    default:
                    {
                        speed = get_speed();
                        display_dashboard(event,speed);
                    }
                }
                break;
            case LOGIN_FLAG:
            {
                unsigned char * h = read_log_car_event(0);
                
                clcd_print(h, LINE2(0));
                if(key == SW6)
                {
                    control_flag = VIEW_LOGS_FLAG;
                } 
                        
                break;
            }
            case VIEW_LOGS_FLAG:
            {
                
                if(key == SW4)
                {
                    logs_index = display_log_car_event(logs_index+1)==-1?logs_index:logs_index+1;
                }
                else if(key == SW5)
                {
                    logs_index = display_log_car_event(logs_index-1)==-1?logs_index:logs_index-1;
                }
                else if(key == SW6)
                {
                    control_flag = DASHBOARD_FLAG;
                    logs_index = 0;
                }
                else{
                    display_log_car_event(logs_index);
                }
                break;
            }
            default:
                
                break;
        }
    }
    return;
}
