#include "car_black_box_de.h"

//Holds the time in formart
//0  1  2
//HH MM SS
unsigned char clock_reg[3];

//Where the time in ASCII is stored
char time[6];

//Current Index of the Logged events
int log_index = -1;

//Number of events Logged
char no_events = 0;

//Log Event
unsigned char log[11];

//edited log car event


//menu options
char *menu_options[] = {"View Logs ", "Clear Logs", "Change Passwrd"};

extern unsigned char sec, return_time;

void get_time() {
    clock_reg[0] = read_ds1307(HOUR_ADDR); // HH -> BCD 
    clock_reg[1] = read_ds1307(MIN_ADDR); // MM -> BCD 
    clock_reg[2] = read_ds1307(SEC_ADDR); // SS -> BCD 

    /* To store the time in HH:MM:SS format */
    // Hour -> 
    time[0] = ((clock_reg[0] >> 4) & 0x03) + '0';
    time[1] = (clock_reg[0] & 0x0F) + '0';

    // Minute
    time[2] = ((clock_reg[1] >> 4) & 0x07) + '0';
    time[3] = (clock_reg[1] & 0x0F) + '0';

    // Second
    time[4] = ((clock_reg[2] >> 4) & 0x07) + '0';
    time[5] = (clock_reg[2] & 0x0F) + '0';
}

void display_time() {
    get_time();


    clcd_putch(time[0], LINE2(2));
    clcd_putch(time[1], LINE2(3));
    clcd_putch(':', LINE2(4));

    clcd_putch(time[2], LINE2(5));
    clcd_putch(time[3], LINE2(6));
    clcd_putch(':', LINE2(7));

    clcd_putch(time[4], LINE2(8));
    clcd_putch(time[5], LINE2(9));
}

unsigned char get_speed(void) {
    return (unsigned char)((float) read_adc() / 1023 * 99);
}

void display_dashboard(char* event, unsigned char speed) {
    //Print header line
    clcd_print("  TIME     E  SP", LINE1(0));

    clcd_print("  ", LINE2(0));

    //Display time in format HH:MM:SS
    display_time();


    //Display Event
    clcd_putch(event[0], LINE2(11));
    clcd_putch(event[1], LINE2(12));
    clcd_putch(event[2], LINE2(13));

    //Display Speed
    clcd_putch((speed / 10) + '0', LINE2(14));
    clcd_putch((speed % 10) + '0', LINE2(15));
}

void log_event(void) {
    log_index++;
    if (log_index > 9) {
        log_index = 0;
    }

    unsigned char addr = (unsigned char) log_index * 10 + 0x05;

    for (int i = 0; log[i] != '\0'; i++) {
        eeprom_write(addr + (unsigned char) i, log[i]);
    }
    if (no_events < 10)
        no_events++;
}

unsigned char* read_log_car_event(char index) {

    if (index < no_events) {
        //                       "9 20:17:99 ON 44"
        unsigned char edited[16];
        unsigned char addr = index * 10 + 0x05;
        for (int i = 0; i < 10; i++) {
            log[i] = eeprom_read(addr + (unsigned char) i);
        }
        edited[0] = index + '0';
        edited[1] = ' ';
        edited[2] = log[0];
        edited[3] = log[1];
        edited[4] = ':';
        edited[5] = log[2];
        edited[6] = log[3];
        edited[7] = ':';
        edited[8] = log[4];
        edited[9] = log[5];
        edited[10] = ' ';
        edited[11] = log[6];
        edited[12] = log[7];
        edited[13] = ' ';
        edited[14] = log[8];
        edited[15] = log[9];
        return edited;
    }
    return NULL;

}

void log_car_event(char event[], unsigned char speed) {
    get_time();
    //HHMMSSEESP
    //0123456789
    strncpy((char*)log, time, 6);

    strncpy((char*)&log[6], event, 2);

    //Write Speed
    log[8] = (speed / 10) + '0';
    log[9] = (speed % 10) + '0';

    log[10] = '\0';

    log_event();
}

char display_log_car_event(char index) {
    if (no_events == 0) {
        //Print header line
        clcd_print("#  TIME    E   SP", LINE1(0));


        clcd_print("No Logs Available", LINE2(0));
        return 1;
    }
    if (index < no_events) {
        //Print header line
        clcd_print("#  TIME    E   SP", LINE1(0));


        clcd_print(read_log_car_event(index), LINE2(0));
        return 1;
    }
    return 0;

}

void clcd_clear_screen(void) {
    clcd_write(CLEAR_DISP_SCREEN, INST_MODE);
    __delay_us(100);
}

char login(unsigned char key, unsigned char reset_flag) {
    //Password entered
    static char npassword[4];
    //Number of attempts remaining
    static unsigned char attempt_rem, i;
    unsigned char same_password;

    if (reset_flag == RESET_PASSWORD) {
        attempt_rem = '3';
        return_time = 5;
        i = 0;

        npassword[0] = '\0';
        npassword[1] = '\0';
        npassword[2] = '\0';
        npassword[3] = '\0';
        key = ALL_RELEASED;
    } else {
        if (return_time < 1) {
            return RETURN_BACK;
        }
        if (i < 4) {
            if (key == SW4) {
                npassword[i] = PASS_SW4;
                clcd_putch('*', LINE2(6 + i));
                return_time = 5;
                i++;
            } else if (key == SW5) {
                npassword[i] = PASS_SW5;
                clcd_putch('*', LINE2(6 + i));
                return_time = 5;
                i++;
            }
        }
        if (i == 4) {
            /*
             * for(int j = 0; (j < 4) ;j++)
             *  {
             *       spassword[i] = eeprom_read(j);
             *  }
             * if(strncmp(npassword, spassword, 4) == 0)
             * {
             * 
             * }`
             * 
             */
            same_password = 1;
            for (char j = 0; (j < 4) && (same_password == 1); j++) {
                same_password = eeprom_read(j) == npassword[j] ? 1 : 0;
            }
            if (same_password == 1)//Menu Screen
            {
                return LOGIN_SUCCESS;
            } else//Wrong password
            {
                attempt_rem--;
                if (attempt_rem == '0')//Block user for 15 minutes
                {
                    clcd_clear_screen();
                    clcd_print(" You are blocked ", LINE1(0));
                    clcd_print(" Wait for 60 seconds... ", LINE2(0));
                    sec = 60;

                    clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
                    __delay_us(100);
                    while (sec != 0) {
                        clcd_putch((sec / 10) + '0', LINE2(10));
                        clcd_putch((sec % 10) + '0', LINE2(11));
                    }
                    clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
                    __delay_us(100);
                    /* Switching off the Timer2 Interrupt */
                    attempt_rem = '3';
                } else {
                    clcd_clear_screen();
                    clcd_print(" WRONG PASSWORD ", LINE1(0));
                    clcd_putch(attempt_rem, LINE2(0));
                    clcd_print(" attempts remaining", LINE2(1));
                    __delay_ms(3000);
                }
                clcd_clear_screen();
                clcd_print(" ENTER PASSWORD", LINE1(0));
                clcd_print(" ", LINE2(CURSOR_POS));
                i = 0;
                return_time = 5;
            }
        }
    }
    return 0x98;

}
//char *menu_options[]  = {"View Logs ", "Clear Logs", "Change Password"};

char login_menu(unsigned char key, unsigned char reset_flag) {
    static char menu_pos;

    if (reset_flag == RESET_LOGIN_MENU) {
        menu_pos = 0;
    }

    if (key == SW5 && menu_pos < 2)//Down Key- Next Option
    {
        menu_pos++;
        clcd_clear_screen();
    } else if (key == SW4 && menu_pos > 0)//Up Key- Previous Option
    {
        menu_pos--;
        clcd_clear_screen();
    }
    if (menu_pos < 2) {
        clcd_putch('*', menu_pos == 0 ? LINE1(0) : LINE2(0));
        clcd_print(menu_options[0], LINE1(2));
        clcd_print(menu_options[1], LINE2(2));
    } else if (menu_pos == 2) {
        clcd_putch('*', LINE2(0));
        clcd_print(menu_options[menu_pos - 1], LINE1(2));
        clcd_print(menu_options[menu_pos], LINE2(2));
    }

    return menu_pos;
}

char clear_logs(unsigned char reset_flag) {
    if(reset_flag == RESET_CLEAR_LOGS)
    {
        log_index = -1;
        no_events = 0;

        clcd_print("  Logs Cleared  ", LINE1(0));
        clcd_print("  successfully  ", LINE2(0));
        sec = 3;
        TMR2ON = 1;
        while (sec > 0) {
            ;
        }
        TMR2ON = 0;
        return TASK_SUCCESS;
    }

    return TASK_FAIL;


}

char change_password(unsigned char key, unsigned char reset_flag)
{
    static char pwd[8];
    static int pos;
    if(reset_flag == RESET_CHANGE_PASSWORD)
    {
        pos = -1;
        clcd_clear_screen();
        clcd_write(DISP_ON_AND_CURSOR_ON, INST_MODE);
        __delay_us(100);
    }
    //Entering Password First
    if(pos < 3)
    {
        clcd_print("Type in New PWD", LINE1(0));
        if (key == SW4) {
            pos++;
            pwd[pos] = PASS_SW4;
            clcd_putch('*', LINE2((char) (6 + pos)));
            return 0x01;

        } else if (key == SW5) {
            pos++;
            pwd[pos] = PASS_SW5;
            clcd_putch('*', LINE2((char) (6 + pos)));
            return 0x01;
        }
    }
    else if(pos < 7) // Entering Password Again
    {
        clcd_print("Re-Enter new PWD", LINE1(0));
        if(pos == 3)
        {
            clcd_print("      ", LINE2((char) (2 + pos)));
        }
        if (key == SW4) {
            pos++;
            pwd[pos] = PASS_SW4;
            clcd_print("*   ", LINE2((char) (2 + pos)));
            return 0x01;

        } else if (key == SW5) {
            pos++;
            pwd[pos] = PASS_SW5;
            clcd_print("*   ", LINE2((char) (2 + pos)));
            return 0x01;
        }
    }
    else //Entered Password Twice
    {
        clcd_write(DISP_ON_AND_CURSOR_OFF, INST_MODE);
        __delay_us(100);
        if(pwd[0] == pwd[4] && pwd[1] == pwd[5] && pwd[2] == pwd[6] && pwd[3] == pwd[7])
        {
            //Password Change Successful
            for(char i =0;i<4;i++)
                eeprom_write(i,pwd[i]);
            clcd_clear_screen();
            clcd_print(" Password change", LINE1(0));
            clcd_print("    successful", LINE2(0));
            sec = 3;
            TMR2ON = 1;
            while (sec > 0) {
                ;
            }
            TMR2ON = 0;
            pos = -1;
            return TASK_SUCCESS;
        }
        else{
            //Password Change Failed
            clcd_clear_screen();
            clcd_print(" Password change", LINE1(0));
            clcd_print("    failed", LINE2(0));
            sec = 3;
            TMR2ON = 1;
            while (sec > 0) {
                ;
            }
            TMR2ON = 0;
            pos = -1;
            return TASK_FAIL;
        }
    }
    return 0x00;
}