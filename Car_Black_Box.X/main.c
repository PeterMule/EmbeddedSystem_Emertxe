/* 
 * File:   main.c
 * Author: PETER
 *
 * Created on March 23, 2022, 4:49 PM
 */

#include "main.h"


#pragma config WDTE = OFF        // Watchdog Timer Enable bit (WDT disabled)


char* gear[] = {"ON","C ", "GN","GR","G1","G2","G3","G4"};


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
    
    //
    init_timer2();
    
    PEIE = 1;
    
    GIE = 1;
    
}

void main(void) {
    
    init_config();
    
    
    unsigned char speed = get_speed();
    unsigned char key,reset_flag;
    unsigned char gr = 0;
    char event[3];
    strcpy(event,gear[gr]);
    log_car_event(event,speed);
    unsigned char control_flag = DASHBOARD_FLAG;
    char logs_index = 0,menu_pos;
    
    //SET initial password as "0100"
    eeprom_write(0x00,PASS_SW4);
    eeprom_write(0x01,PASS_SW5);
    eeprom_write(0x02,PASS_SW4);
    eeprom_write(0x03,PASS_SW4);
    
    
    
    while(1)
    {
        key = read_digital_keypad(STATE);
        
        //Remove bouncing Effect
        for(int i = 0; i<3000;i++);

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
                        if (gr < 7)
                        {
                            strcpy(event,gear[gr<2?gr=2:++gr]);
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
                        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                        __delay_us(100);
                        control_flag = LOGIN_FLAG;
                        reset_flag = RESET_PASSWORD;
                        login(key,reset_flag);
                        reset_flag = RESET_NOTHING;
                        TMR2ON = 1;
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
                switch(login(key,reset_flag))
                {
                    case RETURN_BACK:
                    {
                        control_flag = DASHBOARD_FLAG;
                        TMR2ON = 0;
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        break;
                    }
                    case LOGIN_SUCCESS:
                    {
                        control_flag = MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        login_menu(ALL_RELEASED,reset_flag);
                        reset_flag = RESET_NOTHING;
                        clcd_clear_screen();
                        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                        __delay_us(100);
                        break;
                    }
                }
                
                /*
                unsigned char * h = read_log_car_event(0);
                
                clcd_print(h, LINE2(0));
                if(key == SW6)
                {
                    control_flag = VIEW_LOGS_FLAG;
                } 
                */
                break;
            }
            case MENU_FLAG:
            {
                menu_pos = login_menu(key,reset_flag);
                if(key == SW6){
                    switch(menu_pos)
                    {
                        case 0:
                        {
                            control_flag = VIEW_LOGS_FLAG;
                            logs_index = 0;
                            break;
                        }
                        case 1:
                        {
                            control_flag = CLEAR_LOGS_FLAG;
                            reset_flag = RESET_CLEAR_LOGS;
                            break;
                        }
                        case 2:
                        {
                            control_flag = CHANGE_PASS_FLAG;
                            speed = get_speed();
                            log_car_event("CP", speed);
                            reset_flag = RESET_CHANGE_PASSWORD;
                            change_password(ALL_RELEASED, reset_flag);
                            reset_flag = RESET_NOTHING;
                            
                            break;
                        }
                    }
                }
                if(key == SW1 || key == SW2 || key == SW3)
                {
                    clcd_clear_screen();
                    control_flag = DASHBOARD_FLAG;
                }
                break;
            }
            case VIEW_LOGS_FLAG:
            {
                if(key == SW4)
                {
                    logs_index = display_log_car_event(logs_index+1)==0?logs_index:logs_index+1;
                }
                else if(key == SW5)
                {
                    logs_index = display_log_car_event(logs_index-1)==0?logs_index:logs_index-1;
                }
                else if(key == SW6)
                {
                    control_flag = MENU_FLAG;
                    logs_index = 0;
                    clcd_clear_screen();
                }
                else{
                    display_log_car_event(logs_index);
                }
                break;
            }
            case CLEAR_LOGS_FLAG:
            {
                clcd_clear_screen();
                if(clear_logs(reset_flag) == TASK_SUCCESS)
                {
                    clcd_clear_screen();
                    control_flag = MENU_FLAG;   
                    reset_flag = RESET_LOGIN_MENU;
                    continue;
                }
                break;
            }
            case CHANGE_PASS_FLAG:
            {
                switch(change_password(key, reset_flag))
                {
                    case TASK_SUCCESS:
                    {
                        clcd_clear_screen();
                        control_flag = MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        login_menu(ALL_RELEASED,reset_flag);
                        reset_flag = RESET_NOTHING;
                        break;
                    }
                    case TASK_FAIL:
                    {
                        clcd_clear_screen();
                        control_flag = MENU_FLAG;
                        reset_flag = RESET_LOGIN_MENU;
                        login_menu(ALL_RELEASED,reset_flag);
                        reset_flag = RESET_NOTHING;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                
                break;
            }
            default:
                
                break;
        }
        
    }
    return;
}
